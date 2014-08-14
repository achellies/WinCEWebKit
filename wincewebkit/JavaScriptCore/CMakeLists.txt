SET(JavaScriptCore_INCLUDE_DIRECTORIES
    "${CMAKE_BINARY_DIR}"
    "${JAVASCRIPTCORE_DIR}"
    "${JAVASCRIPTCORE_DIR}/API"
    "${JAVASCRIPTCORE_DIR}/ForwardingHeaders"
    "${JAVASCRIPTCORE_DIR}/assembler"
    "${JAVASCRIPTCORE_DIR}/bytecode"
    "${JAVASCRIPTCORE_DIR}/bytecompiler"
    "${JAVASCRIPTCORE_DIR}/debugger"
    "${JAVASCRIPTCORE_DIR}/interpreter"
    "${JAVASCRIPTCORE_DIR}/jit"
    "${JAVASCRIPTCORE_DIR}/parser"
    "${JAVASCRIPTCORE_DIR}/pcre"
    "${JAVASCRIPTCORE_DIR}/profiler"
    "${JAVASCRIPTCORE_DIR}/runtime"
    "${JAVASCRIPTCORE_DIR}/yarr"
    "${WTF_INCLUDE_DIRECTORIES}"
    "${CMAKE_SOURCE_DIR}"
)

SET(JavaScriptCore_HEADERS
    API/APICast.h
    API/APIShims.h
    API/JavaScript.h
    API/JavaScriptCore.h
    API/JSBase.h
    API/JSBasePrivate.h
    API/JSCallbackConstructor.h
    API/JSCallbackFunction.h
    API/JSCallbackObject.h
    API/JSCallbackObjectFunctions.h
    API/JSClassRef.h
    API/JSContextRef.h
    API/JSContextRefPrivate.h
    API/JSObjectRef.h
    API/JSObjectRefPrivate.h
    API/JSProfilerPrivate.h
    API/JSRetainPtr.h
    API/JSStringRef.h
    API/JSValueRef.h
    API/JSWeakObjectMapRefInternal.h
    API/JSWeakObjectMapRefPrivate.h
    API/OpaqueJSString.h
    API/WebKitAvailability.h

    assembler/AbstractMacroAssembler.h
    assembler/AssemblerBuffer.h
    assembler/AssemblerBufferWithConstantPool.h
    assembler/CodeLocation.h
    assembler/LinkBuffer.h
    assembler/MacroAssembler.h
    assembler/MacroAssemblerCodeRef.h
    assembler/RepatchBuffer.h

    bytecode/CodeBlock.h
    bytecode/EvalCodeCache.h
    bytecode/Instruction.h
    bytecode/JumpTable.h
    bytecode/Opcode.h
    bytecode/SamplingTool.h
    bytecode/StructureStubInfo.h

    bytecompiler/BytecodeGenerator.h
    bytecompiler/Label.h
    bytecompiler/LabelScope.h
    bytecompiler/RegisterID.h

    debugger/Debugger.h
    debugger/DebuggerActivation.h
    debugger/DebuggerCallFrame.h

    interpreter/CachedCall.h
    interpreter/CallFrame.h
    interpreter/CallFrameClosure.h
    interpreter/Interpreter.h
    interpreter/Register.h
    interpreter/RegisterFile.h

    jit/ExecutableAllocator.h
    jit/JIT.h
    jit/JITCode.h
    jit/JITInlineMethods.h
    jit/JITStubCall.h
    jit/JITStubs.h
    jit/JSInterfaceJIT.h
    jit/SpecializedThunkJIT.h
    jit/ThunkGenerators.h

    parser/Lexer.h
    parser/NodeConstructors.h
    parser/NodeInfo.h
    parser/Nodes.h
    parser/Parser.h
    parser/ParserArena.h
    parser/ResultType.h
    parser/SourceCode.h
    parser/SourceProvider.h

    pcre/pcre.h
    pcre/pcre_internal.h
    pcre/ucpinternal.h

    profiler/CallIdentifier.h
    profiler/Profile.h
    profiler/ProfileGenerator.h
    profiler/ProfileNode.h
    profiler/Profiler.h
    profiler/ProfilerServer.h

    runtime/ArgList.h
    runtime/Arguments.h
    runtime/ArrayConstructor.h
    runtime/ArrayPrototype.h
    runtime/BatchedTransitionOptimizer.h
    runtime/BooleanConstructor.h
    runtime/BooleanObject.h
    runtime/BooleanPrototype.h
    runtime/CallData.h
    runtime/ClassInfo.h
    runtime/Collector.h
    runtime/CollectorHeapIterator.h
    runtime/CommonIdentifiers.h
    runtime/Completion.h
    runtime/ConstructData.h
    runtime/DateConstructor.h
    runtime/DateConversion.h
    runtime/DateInstance.h
    runtime/DateInstanceCache.h
    runtime/DatePrototype.h
    runtime/Error.h
    runtime/ErrorConstructor.h
    runtime/ErrorInstance.h
    runtime/ErrorPrototype.h
    runtime/ExceptionHelpers.h
    runtime/Executable.h
    runtime/FunctionConstructor.h
    runtime/FunctionPrototype.h
    runtime/GetterSetter.h
    runtime/GlobalEvalFunction.h
    runtime/Identifier.h
    runtime/InitializeThreading.h
    runtime/InternalFunction.h
    runtime/JSActivation.h
    runtime/JSAPIValueWrapper.h
    runtime/JSArray.h
    runtime/JSByteArray.h
    runtime/JSCell.h
    runtime/JSFunction.h
    runtime/JSGlobalData.h
    runtime/JSGlobalObject.h
    runtime/JSGlobalObjectFunctions.h
    runtime/JSImmediate.h
    runtime/JSLock.h
    runtime/JSNotAnObject.h
    runtime/JSNumberCell.h
    runtime/JSObject.h
    runtime/JSObjectWithGlobalObject.h
    runtime/JSONObject.h
    runtime/JSPropertyNameIterator.h
    runtime/JSStaticScopeObject.h
    runtime/JSString.h
    runtime/JSStringBuilder.h
    runtime/JSType.h
    runtime/JSTypeInfo.h
    runtime/JSValue.h
    runtime/JSVariableObject.h
    runtime/JSWrapperObject.h
    runtime/JSZombie.h
    runtime/LiteralParser.h
    runtime/Lookup.h
    runtime/MarkStack.h
    runtime/MathObject.h
    runtime/NativeErrorConstructor.h
    runtime/NativeErrorPrototype.h
    runtime/NativeFunctionWrapper.h
    runtime/NumberConstructor.h
    runtime/NumberObject.h
    runtime/NumberPrototype.h
    runtime/NumericStrings.h
    runtime/ObjectConstructor.h
    runtime/ObjectPrototype.h
    runtime/Operations.h
    runtime/PropertyDescriptor.h
    runtime/PropertyMapHashTable.h
    runtime/PropertyNameArray.h
    runtime/PropertySlot.h
    runtime/Protect.h
    runtime/PrototypeFunction.h
    runtime/PutPropertySlot.h
    runtime/RegExp.h
    runtime/RegExpConstructor.h
    runtime/RegExpMatchesArray.h
    runtime/RegExpObject.h
    runtime/RegExpPrototype.h
    runtime/RopeImpl.h
    runtime/ScopeChain.h
    runtime/ScopeChainMark.h
    runtime/SmallStrings.h
    runtime/StringConstructor.h
    runtime/StringObject.h
    runtime/StringObjectThatMasqueradesAsUndefined.h
    runtime/StringPrototype.h
    runtime/Structure.h
    runtime/StructureChain.h
    runtime/StructureTransitionTable.h
    runtime/SymbolTable.h
    runtime/Terminator.h
    runtime/TimeoutChecker.h
    runtime/Tracing.h
    runtime/UString.h
    runtime/WeakGCMap.h
    runtime/WeakGCPtr.h
    runtime/WeakRandom.h

    yarr/RegexCompiler.h
    yarr/RegexInterpreter.h
    yarr/RegexJIT.h
    yarr/RegexParser.h
    yarr/RegexPattern.h

    wtf/DateMath.h
)

SET(JavaScriptCore_SOURCES
    API/JSBase.cpp
    API/JSCallbackConstructor.cpp
    API/JSCallbackFunction.cpp
    API/JSCallbackObject.cpp
    API/JSClassRef.cpp
    API/JSContextRef.cpp
    API/JSObjectRef.cpp
    API/JSProfilerPrivate.cpp
    API/JSStringRef.cpp
    API/JSValueRef.cpp
    API/JSWeakObjectMapRefPrivate.cpp
    API/OpaqueJSString.cpp

    bytecode/CodeBlock.cpp
    bytecode/JumpTable.cpp
    bytecode/Opcode.cpp
    bytecode/SamplingTool.cpp
    bytecode/StructureStubInfo.cpp

    bytecompiler/BytecodeGenerator.cpp
    bytecompiler/NodesCodegen.cpp

    debugger/Debugger.cpp
    debugger/DebuggerActivation.cpp
    debugger/DebuggerCallFrame.cpp

    interpreter/CallFrame.cpp
    interpreter/Interpreter.cpp
    interpreter/RegisterFile.cpp

    jit/ExecutableAllocator.cpp
    jit/JITArithmetic32_64.cpp
    jit/JITArithmetic.cpp
    jit/JITCall32_64.cpp
    jit/JITCall.cpp
    jit/JIT.cpp
    jit/JITOpcodes32_64.cpp
    jit/JITOpcodes.cpp
    jit/JITPropertyAccess32_64.cpp
    jit/JITPropertyAccess.cpp
    jit/JITStubs.cpp
    jit/ThunkGenerators.cpp

    parser/JSParser.cpp
    parser/Lexer.cpp
    parser/Nodes.cpp
    parser/Parser.cpp
    parser/ParserArena.cpp

    pcre/pcre_compile.cpp
    pcre/pcre_exec.cpp
    pcre/pcre_tables.cpp
    pcre/pcre_ucp_searchfuncs.cpp
    pcre/pcre_xclass.cpp

    profiler/Profile.cpp
    profiler/ProfileGenerator.cpp
    profiler/ProfileNode.cpp
    profiler/Profiler.cpp

    runtime/ArgList.cpp
    runtime/Arguments.cpp
    runtime/ArrayConstructor.cpp
    runtime/ArrayPrototype.cpp
    runtime/BooleanConstructor.cpp
    runtime/BooleanObject.cpp
    runtime/BooleanPrototype.cpp
    runtime/CallData.cpp
    runtime/Collector.cpp
    runtime/CommonIdentifiers.cpp
    runtime/Completion.cpp
    runtime/ConstructData.cpp
    runtime/DateConstructor.cpp
    runtime/DateConversion.cpp
    runtime/DateInstance.cpp
    runtime/DatePrototype.cpp
    runtime/Error.cpp
    runtime/ErrorConstructor.cpp
    runtime/ErrorInstance.cpp
    runtime/ErrorPrototype.cpp
    runtime/ExceptionHelpers.cpp
    runtime/Executable.cpp
    runtime/FunctionConstructor.cpp
    runtime/FunctionPrototype.cpp
    runtime/GCActivityCallback.cpp
    runtime/GCHandle.cpp
    runtime/GetterSetter.cpp
    runtime/GlobalEvalFunction.cpp
    runtime/Identifier.cpp
    runtime/InitializeThreading.cpp
    runtime/InternalFunction.cpp
    runtime/JSActivation.cpp
    runtime/JSAPIValueWrapper.cpp
    runtime/JSArray.cpp
    runtime/JSByteArray.cpp
    runtime/JSCell.cpp
    runtime/JSFunction.cpp
    runtime/JSGlobalData.cpp
    runtime/JSGlobalObject.cpp
    runtime/JSGlobalObjectFunctions.cpp
    runtime/JSImmediate.cpp
    runtime/JSLock.cpp
    runtime/JSNotAnObject.cpp
    runtime/JSNumberCell.cpp
    runtime/JSObject.cpp
    runtime/JSObjectWithGlobalObject.cpp
    runtime/JSONObject.cpp
    runtime/JSPropertyNameIterator.cpp
    runtime/JSStaticScopeObject.cpp
    runtime/JSString.cpp
    runtime/JSValue.cpp
    runtime/JSVariableObject.cpp
    runtime/JSWrapperObject.cpp
    runtime/JSZombie.cpp
    runtime/LiteralParser.cpp
    runtime/Lookup.cpp
    runtime/MarkStack.cpp
    runtime/MathObject.cpp
    runtime/NativeErrorConstructor.cpp
    runtime/NativeErrorPrototype.cpp
    runtime/NumberConstructor.cpp
    runtime/NumberObject.cpp
    runtime/NumberPrototype.cpp
    runtime/ObjectConstructor.cpp
    runtime/ObjectPrototype.cpp
    runtime/Operations.cpp
    runtime/PropertyDescriptor.cpp
    runtime/PropertyNameArray.cpp
    runtime/PropertySlot.cpp
    runtime/PrototypeFunction.cpp
    runtime/RegExp.cpp
    runtime/RegExpCache.cpp
    runtime/RegExpConstructor.cpp
    runtime/RegExpObject.cpp
    runtime/RegExpPrototype.cpp
    runtime/RopeImpl.cpp
    runtime/ScopeChain.cpp
    runtime/SmallStrings.cpp
    runtime/StrictEvalActivation.cpp
    runtime/StringConstructor.cpp
    runtime/StringObject.cpp
    runtime/StringPrototype.cpp
    runtime/Structure.cpp
    runtime/StructureChain.cpp
    runtime/TimeoutChecker.cpp
    runtime/UString.cpp

    yarr/RegexCompiler.cpp
    yarr/RegexInterpreter.cpp
    yarr/RegexJIT.cpp

    wtf/DateMath.cpp
    wtf/PageAllocation.cpp
)

SET(JavaScriptCore_LUT_FILES
    runtime/ArrayPrototype.cpp
    runtime/DatePrototype.cpp
    runtime/JSONObject.cpp
    runtime/MathObject.cpp
    runtime/NumberConstructor.cpp
    runtime/RegExpConstructor.cpp
    runtime/RegExpObject.cpp
    runtime/StringPrototype.cpp
)

SET(JavaScriptCore_LIBRARIES
    ${WTF_LIBRARY_NAME}
)


# GENERATOR 1-A: LUT creator
FOREACH (_file ${JavaScriptCore_LUT_FILES})
    GET_FILENAME_COMPONENT(_name ${_file} NAME_WE)
    GENERATE_HASH_LUT(${JAVASCRIPTCORE_DIR}/${_file} ${DERIVED_SOURCES_DIR}/${_name}.lut.h)
    LIST(APPEND JavaScriptCore_HEADERS ${DERIVED_SOURCES_DIR}/${_name}.lut.h)
ENDFOREACH ()


# GENERATOR 1-B: particular LUT creator (for 1 file only)
GENERATE_HASH_LUT(${JAVASCRIPTCORE_DIR}/parser/Keywords.table ${DERIVED_SOURCES_DIR}/Lexer.lut.h MAIN_DEPENDENCY)
LIST(APPEND JavaScriptCore_HEADERS ${DERIVED_SOURCES_DIR}/Lexer.lut.h)

# GENERATOR: "chartables.c": compile and execute the chartables generator (and add it to sources)
ADD_CUSTOM_COMMAND(
    OUTPUT ${DERIVED_SOURCES_DIR}/chartables.c
    MAIN_DEPENDENCY ${JAVASCRIPTCORE_DIR}/pcre/dftables
    COMMAND ${PERL_EXECUTABLE} ${JAVASCRIPTCORE_DIR}/pcre/dftables --preprocessor "${CODE_GENERATOR_PREPROCESSOR}" ${DERIVED_SOURCES_DIR}/chartables.c
    VERBATIM)
ADD_SOURCE_DEPENDENCIES(${JAVASCRIPTCORE_DIR}/pcre/pcre_tables.cpp ${DERIVED_SOURCES_DIR}/chartables.c)


#GENERATOR: "RegExpJitTables.h": tables used by Yarr
ADD_CUSTOM_COMMAND(
    OUTPUT ${DERIVED_SOURCES_DIR}/RegExpJitTables.h
    MAIN_DEPENDENCY ${JAVASCRIPTCORE_DIR}/create_regex_tables
    COMMAND ${PYTHON_EXECUTABLE} ${JAVASCRIPTCORE_DIR}/create_regex_tables > ${DERIVED_SOURCES_DIR}/RegExpJitTables.h
    VERBATIM)
ADD_SOURCE_DEPENDENCIES(${JAVASCRIPTCORE_DIR}/yarr/RegexCompiler.cpp ${DERIVED_SOURCES_DIR}/RegExpJitTables.h)



IF (WTF_CPU_ARM)
    LIST(APPEND JavaScriptCore_SOURCES
        assembler/ARMAssembler.cpp
        assembler/MacroAssemblerARM.cpp
    )
ELSEIF (WTF_CPU_MIPS)
ELSEIF (WTF_CPU_X86)
ELSEIF (WTF_CPU_X86_64)
ELSE ()
    MESSAGE(FATAL_ERROR "Unknown CPU")
ENDIF ()


INCLUDE_IF_EXISTS(${JAVASCRIPTCORE_DIR}/CMakeLists${PORT}.txt)


# Declare source groups for Visual Studio folders
SOURCE_GROUP(API "API/")
SOURCE_GROUP(assembler "assembler/")
SOURCE_GROUP(bytecode "bytecode/")
SOURCE_GROUP(bytecompiler "bytecompiler/")
SOURCE_GROUP(debugger "debugger/")
SOURCE_GROUP(bytecode "bytecode/")
SOURCE_GROUP(interpreter "interpreter/")
SOURCE_GROUP(jit "jit/")
SOURCE_GROUP(parser "parser/")
SOURCE_GROUP(pcre "pcre/")
SOURCE_GROUP(profiler "profiler/")
SOURCE_GROUP(runtime "runtime/")
SOURCE_GROUP(yarr "yarr/")
SOURCE_GROUP(wtf "wtf/")

ADD_SUBDIRECTORY(wtf)
ADD_SUBDIRECTORY(shell)

WEBKIT_WRAP_SOURCELIST(${JavaScriptCore_SOURCES})
INCLUDE_DIRECTORIES(${JavaScriptCore_INCLUDE_DIRECTORIES})
ADD_DEFINITIONS(-DBUILDING_JavaScriptCore)
ADD_LIBRARY(${JavaScriptCore_LIBRARY_NAME} ${JavaScriptCore_LIBRARY_TYPE} ${JavaScriptCore_HEADERS} ${JavaScriptCore_SOURCES})
TARGET_LINK_LIBRARIES(${JavaScriptCore_LIBRARY_NAME} ${JavaScriptCore_LIBRARIES})

IF (JavaScriptCore_LINK_FLAGS)
    ADD_TARGET_PROPERTIES(${JavaScriptCore_LIBRARY_NAME} LINK_FLAGS "${JavaScriptCore_LINK_FLAGS}")
ENDIF ()

IF (SHARED_CORE)
    SET_TARGET_PROPERTIES(${JavaScriptCore_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
    INSTALL(TARGETS ${JavaScriptCore_LIBRARY_NAME} DESTINATION lib)
ENDIF ()
