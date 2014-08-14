SET(WTF_INCLUDE_DIRECTORIES
    "${JAVASCRIPTCORE_DIR}"
    "${WTF_DIR}"
    "${WTF_DIR}/unicode"
    "${DERIVED_SOURCES_DIR}"
)

SET(WTF_HEADERS
    ASCIICType.h
    AVLTree.h
    AlwaysInline.h
    Assertions.h
    ByteArray.h
    Complex.h
    CrossThreadRefCounted.h
    CurrentTime.h
    Deque.h
    DisallowCType.h
    FastAllocBase.h
    FastMalloc.h
    Forward.h
    GetPtr.h
    HashCountedSet.h
    HashFunctions.h
    HashIterators.h
    HashMap.h
    HashSet.h
    HashTable.h
    HashTraits.h
    ListHashSet.h
    ListRefPtr.h
    Locker.h
    MD5.h
    MainThread.h
    MallocZoneSupport.h
    MathExtras.h
    MessageQueue.h
    Noncopyable.h
    NotFound.h
    OwnArrayPtr.h
    OwnFastMallocPtr.h
    OwnPtr.h
    OwnPtrCommon.h
    PassOwnPtr.h
    PassRefPtr.h
    Platform.h
    PossiblyNull.h
    RandomNumber.h
    RandomNumberSeed.h
    RefCounted.h
    RefCountedLeakCounter.h
    RefPtr.h
    RefPtrHashMap.h
    RetainPtr.h
    SegmentedVector.h
    StdLibExtras.h
    StringExtras.h
    StringHasher.h
    TCPackedCache.h
    TCPageMap.h
    TCSpinLock.h
    ThreadIdentifierDataPthreads.h
    ThreadSpecific.h
    Threading.h
    TypeTraits.h
    UnusedParam.h
    VMTags.h
    ValueCheck.h
    Vector.h
    Vector3.h
    VectorTraits.h
    WTFThreadData.h
    dtoa.h

    text/AtomicString.h
    text/AtomicStringImpl.h
    text/CString.h
    text/StringBuffer.h
    text/StringHash.h
    text/StringImpl.h
    text/StringImplBase.h
    text/WTFString.h

    unicode/Collator.h
    unicode/UTF8.h
    unicode/Unicode.h
)

SET(WTF_SOURCES
    Assertions.cpp
    ByteArray.cpp
    CurrentTime.cpp
    DecimalNumber.cpp
    FastMalloc.cpp
    HashTable.cpp
    MainThread.cpp
    MD5.cpp
    RandomNumber.cpp
    RefCountedLeakCounter.cpp
    StringExtras.cpp
    Threading.cpp
    TypeTraits.cpp
    WTFThreadData.cpp
    dtoa.cpp

    text/AtomicString.cpp
    text/CString.cpp
    text/StringBuilder.cpp
    text/StringImpl.cpp
    text/StringStatics.cpp
    text/WTFString.cpp

    unicode/UTF8.cpp
)

SET(WTF_LIBRARIES
)

INCLUDE_IF_EXISTS(${JAVASCRIPTCORE_DIR}/wtf/CMakeLists${PORT}.txt)


WEBKIT_WRAP_SOURCELIST(${WTF_SOURCES})
INCLUDE_DIRECTORIES(${WTF_INCLUDE_DIRECTORIES})
ADD_DEFINITIONS(-DBUILDING_WTF)
ADD_LIBRARY(${WTF_LIBRARY_NAME} ${WTF_LIBRARY_TYPE} ${WTF_HEADERS} ${WTF_SOURCES})
TARGET_LINK_LIBRARIES(${WTF_LIBRARY_NAME} ${WTF_LIBRARIES})

IF (WTF_LINK_FLAGS)
    ADD_TARGET_PROPERTIES(${WTF_LIBRARY_NAME} LINK_FLAGS "${WTF_LINK_FLAGS}")
ENDIF ()

IF (SHARED_CORE)
    SET_TARGET_PROPERTIES(${WTF_LIBRARY_NAME} PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR})
    INSTALL(TARGETS ${WTF_LIBRARY_NAME} DESTINATION lib)
ENDIF ()
