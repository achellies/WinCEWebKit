# Copyright (C) 2010 Google Inc. All rights reserved.
# Copyright (C) 2009 Daniel Bates (dbates@intudata.com). All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#    * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#    * Neither the name of Google Inc. nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import signal
import subprocess
import sys
import unittest

from webkitpy.common.system.executive import Executive, run_command, ScriptError
from webkitpy.test import cat, echo


def never_ending_command():
    """Arguments for a command that will never end (useful for testing process
    killing). It should be a process that is unlikely to already be running
    because all instances will be killed."""
    if sys.platform == 'win32':
        return ['wmic']
    return ['yes']


class ExecutiveTest(unittest.TestCase):

    def test_run_command_with_bad_command(self):
        def run_bad_command():
            run_command(["foo_bar_command_blah"], error_handler=Executive.ignore_error, return_exit_code=True)
        self.failUnlessRaises(OSError, run_bad_command)

    def test_run_command_args_type(self):
        executive = Executive()
        self.assertRaises(AssertionError, executive.run_command, "echo")
        self.assertRaises(AssertionError, executive.run_command, u"echo")
        executive.run_command(echo.command_arguments('foo'))
        executive.run_command(tuple(echo.command_arguments('foo')))

    def test_run_command_with_unicode(self):
        """Validate that it is safe to pass unicode() objects
        to Executive.run* methods, and they will return unicode()
        objects by default unless decode_output=False"""
        unicode_tor_input = u"WebKit \u2661 Tor Arne Vestb\u00F8!"
        if sys.platform == 'win32':
            encoding = 'mbcs'
        else:
            encoding = 'utf-8'
        encoded_tor = unicode_tor_input.encode(encoding)
        # On Windows, we expect the unicode->mbcs->unicode roundtrip to be
        # lossy. On other platforms, we expect a lossless roundtrip.
        if sys.platform == 'win32':
            unicode_tor_output = encoded_tor.decode(encoding)
        else:
            unicode_tor_output = unicode_tor_input

        executive = Executive()

        output = executive.run_command(cat.command_arguments(), input=unicode_tor_input)
        self.assertEquals(output, unicode_tor_output)

        output = executive.run_command(echo.command_arguments("-n", unicode_tor_input))
        self.assertEquals(output, unicode_tor_output)

        output = executive.run_command(echo.command_arguments("-n", unicode_tor_input), decode_output=False)
        self.assertEquals(output, encoded_tor)

        # Make sure that str() input also works.
        output = executive.run_command(cat.command_arguments(), input=encoded_tor, decode_output=False)
        self.assertEquals(output, encoded_tor)

        # FIXME: We should only have one run* method to test
        output = executive.run_and_throw_if_fail(echo.command_arguments("-n", unicode_tor_input), quiet=True)
        self.assertEquals(output, unicode_tor_output)

        output = executive.run_and_throw_if_fail(echo.command_arguments("-n", unicode_tor_input), quiet=True, decode_output=False)
        self.assertEquals(output, encoded_tor)

    def test_kill_process(self):
        executive = Executive()
        process = subprocess.Popen(never_ending_command(), stdout=subprocess.PIPE)
        self.assertEqual(process.poll(), None)  # Process is running
        executive.kill_process(process.pid)
        # Note: Can't use a ternary since signal.SIGKILL is undefined for sys.platform == "win32"
        if sys.platform == "win32":
            expected_exit_code = 1
        else:
            expected_exit_code = -signal.SIGKILL
        self.assertEqual(process.wait(), expected_exit_code)
        # Killing again should fail silently.
        executive.kill_process(process.pid)

    def _assert_windows_image_name(self, name, expected_windows_name):
        executive = Executive()
        windows_name = executive._windows_image_name(name)
        self.assertEqual(windows_name, expected_windows_name)

    def test_windows_image_name(self):
        self._assert_windows_image_name("foo", "foo.exe")
        self._assert_windows_image_name("foo.exe", "foo.exe")
        self._assert_windows_image_name("foo.com", "foo.com")
        # If the name looks like an extension, even if it isn't
        # supposed to, we have no choice but to return the original name.
        self._assert_windows_image_name("foo.baz", "foo.baz")
        self._assert_windows_image_name("foo.baz.exe", "foo.baz.exe")

    def test_kill_all(self):
        executive = Executive()
        # We use "yes" because it loops forever.
        process = subprocess.Popen(never_ending_command(), stdout=subprocess.PIPE)
        self.assertEqual(process.poll(), None)  # Process is running
        executive.kill_all(never_ending_command()[0])
        # Note: Can't use a ternary since signal.SIGTERM is undefined for sys.platform == "win32"
        if sys.platform == "cygwin":
            expected_exit_code = 0  # os.kill results in exit(0) for this process.
        elif sys.platform == "win32":
            expected_exit_code = 1
        else:
            expected_exit_code = -signal.SIGTERM
        self.assertEqual(process.wait(), expected_exit_code)
        # Killing again should fail silently.
        executive.kill_all(never_ending_command()[0])

    def test_check_running_pid(self):
        executive = Executive()
        self.assertTrue(executive.check_running_pid(os.getpid()))
        # Maximum pid number on Linux is 32768 by default
        self.assertFalse(executive.check_running_pid(100000))
