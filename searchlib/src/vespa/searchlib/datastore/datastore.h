// Copyright 2016 Yahoo Inc. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#pragma once

#include "datastorebase.h"

namespace search {
namespace btree {

template<typename EntryType>
struct DefaultReclaimer {
    static void reclaim(EntryType *entry) {
        (void) entry;
    }
};

} // namespace btree

namespace datastore {

template <typename RefT = EntryRefT<22> >
class DataStoreT : public DataStoreBase
{
private:
    DataStoreT(const DataStoreT &rhs);

    DataStoreT &
    operator=(const DataStoreT &rhs);
public:
    typedef RefT RefType;

    DataStoreT(void);

    ~DataStoreT(void);

    /**
     * Increase number of dead elements in buffer.
     *
     * @param ref		Reference to dead stored features
     * @param dead		Number of newly dead elements
     */
    void
    incDead(EntryRef ref, uint64_t dead)
    {
        RefType intRef(ref);
        DataStoreBase::incDead(intRef.bufferId(), dead);
    }

    /**
     * Free element.
     */
    void
    freeElem(EntryRef ref, uint64_t len);

    /**
     * Hold element.
     */
    void
    holdElem(EntryRef ref, uint64_t len);

    /**
     * Trim elem hold list, freeing elements that no longer needs to be held.
     *
     * @param usedGen		lowest generation that is still used.
     */
    virtual void
    trimElemHoldList(generation_t usedGen);

    virtual void
    clearElemHoldList(void);

    bool
    getCompacting(EntryRef ref) const
    {
        return getBufferState(RefType(ref).bufferId()).getCompacting();
    }

    template <typename EntryType>
    std::pair<RefType, EntryType *>
    allocNewEntry(uint32_t typeId);

    template <typename EntryType, typename Reclaimer>
    std::pair<RefType, EntryType *>
    allocEntry(uint32_t typeId);

    template <typename EntryType>
    std::pair<RefType, EntryType *>
    allocNewEntryCopy(uint32_t typeId, const EntryType &rhs);

    template <typename EntryType, typename Reclaimer>
    std::pair<RefType, EntryType *>
    allocEntryCopy(uint32_t typeId, const EntryType &rhs);

};


template <typename EntryType, typename RefT = EntryRefT<22> >
class DataStore : public DataStoreT<RefT>
{
private:
    DataStore(const DataStore &rhs);

    DataStore &
    operator=(const DataStore &rhs);
protected:
    typedef DataStoreT<RefT> ParentType;
    using ParentType::ensureBufferCapacity;
    // using ParentType::activeBuffer;
    using ParentType::_activeBufferIds;
    using ParentType::_buffers;
    using ParentType::_states;
    using ParentType::_freeListLists;
    using ParentType::getBufferEntry;
    using ParentType::dropBuffers;
    using ParentType::initActiveBuffers;
    using ParentType::addType;

    BufferType<EntryType> _type;
public:
    typedef typename ParentType::RefType RefType;
    DataStore();

    ~DataStore(void);

    EntryRef
    addEntry(const EntryType &e);

    EntryRef
    addEntry2(const EntryType &e);

    const EntryType &
    getEntry(EntryRef ref) const;
};

extern template class DataStoreT<EntryRefT<22> >;

} // namespace search::datastore
} // namespace search
