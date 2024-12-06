//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Frame.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "Frame_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(Frame)

Frame::Frame(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

Frame::Frame(const Frame& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

Frame::~Frame()
{
    for (size_t i = 0; i < blocks_arraysize; i++)
        drop(&this->blocks[i]);
    delete [] this->blocks;
}

Frame& Frame::operator=(const Frame& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void Frame::copy(const Frame& other)
{
    this->size = other.size;
    for (size_t i = 0; i < blocks_arraysize; i++)
        drop(&this->blocks[i]);
    delete [] this->blocks;
    this->blocks = (other.blocks_arraysize==0) ? nullptr : new Block[other.blocks_arraysize];
    blocks_arraysize = other.blocks_arraysize;
    for (size_t i = 0; i < blocks_arraysize; i++) {
        this->blocks[i] = other.blocks[i];
        this->blocks[i].setName(other.blocks[i].getName());
        take(&this->blocks[i]);
    }
}

void Frame::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->size);
    b->pack(blocks_arraysize);
    doParsimArrayPacking(b,this->blocks,blocks_arraysize);
}

void Frame::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->size);
    delete [] this->blocks;
    b->unpack(blocks_arraysize);
    if (blocks_arraysize == 0) {
        this->blocks = nullptr;
    } else {
        this->blocks = new Block[blocks_arraysize];
        doParsimArrayUnpacking(b,this->blocks,blocks_arraysize);
    }
}

int Frame::getSize() const
{
    return this->size;
}

void Frame::setSize(int size)
{
    this->size = size;
}

size_t Frame::getBlocksArraySize() const
{
    return blocks_arraysize;
}

const Block& Frame::getBlocks(size_t k) const
{
    if (k >= blocks_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)blocks_arraysize, (unsigned long)k);
    return this->blocks[k];
}

void Frame::setBlocksArraySize(size_t newSize)
{
    Block *blocks2 = (newSize==0) ? nullptr : new Block[newSize];
    size_t minSize = blocks_arraysize < newSize ? blocks_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        blocks2[i] = this->blocks[i];
    for (size_t i = 0; i < blocks_arraysize; i++)
        drop(&this->blocks[i]);
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
    for (size_t i = 0; i < blocks_arraysize; i++)
        take(&this->blocks[i]);
}

void Frame::setBlocks(size_t k, const Block& blocks)
{
    if (k >= blocks_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)blocks_arraysize, (unsigned long)k);
    this->blocks[k] = blocks;
}

void Frame::insertBlocks(size_t k, const Block& blocks)
{
    if (k > blocks_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)blocks_arraysize, (unsigned long)k);
    size_t newSize = blocks_arraysize + 1;
    Block *blocks2 = new Block[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        blocks2[i] = this->blocks[i];
    blocks2[k] = blocks;
    for (i = k + 1; i < newSize; i++)
        blocks2[i] = this->blocks[i-1];
    for (size_t i = 0; i < blocks_arraysize; i++)
        drop(&this->blocks[i]);
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
    for (size_t i = 0; i < blocks_arraysize; i++)
        take(&this->blocks[i]);
}

void Frame::appendBlocks(const Block& blocks)
{
    insertBlocks(blocks_arraysize, blocks);
}

void Frame::eraseBlocks(size_t k)
{
    if (k >= blocks_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)blocks_arraysize, (unsigned long)k);
    size_t newSize = blocks_arraysize - 1;
    Block *blocks2 = (newSize == 0) ? nullptr : new Block[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        blocks2[i] = this->blocks[i];
    for (i = k; i < newSize; i++)
        blocks2[i] = this->blocks[i+1];
    for (size_t i = 0; i < blocks_arraysize; i++)
        drop(&this->blocks[i]);
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
    for (size_t i = 0; i < blocks_arraysize; i++)
        take(&this->blocks[i]);
}

class FrameDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_size,
        FIELD_blocks,
    };
  public:
    FrameDescriptor();
    virtual ~FrameDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(FrameDescriptor)

FrameDescriptor::FrameDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Frame)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

FrameDescriptor::~FrameDescriptor()
{
    delete[] propertyNames;
}

bool FrameDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Frame *>(obj)!=nullptr;
}

const char **FrameDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *FrameDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int FrameDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 2+base->getFieldCount() : 2;
}

unsigned int FrameDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_size
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISCOWNEDOBJECT | FD_ISRESIZABLE,    // FIELD_blocks
    };
    return (field >= 0 && field < 2) ? fieldTypeFlags[field] : 0;
}

const char *FrameDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "size",
        "blocks",
    };
    return (field >= 0 && field < 2) ? fieldNames[field] : nullptr;
}

int FrameDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "size") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "blocks") == 0) return baseIndex + 1;
    return base ? base->findField(fieldName) : -1;
}

const char *FrameDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_size
        "Block",    // FIELD_blocks
    };
    return (field >= 0 && field < 2) ? fieldTypeStrings[field] : nullptr;
}

const char **FrameDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *FrameDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int FrameDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_blocks: return pp->getBlocksArraySize();
        default: return 0;
    }
}

void FrameDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_blocks: pp->setBlocksArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Frame'", field);
    }
}

const char *FrameDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string FrameDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_size: return long2string(pp->getSize());
        case FIELD_blocks: return pp->getBlocks(i).str();
        default: return "";
    }
}

void FrameDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_size: pp->setSize(string2long(value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Frame'", field);
    }
}

omnetpp::cValue FrameDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_size: return pp->getSize();
        case FIELD_blocks: return omnetpp::toAnyPtr(&pp->getBlocks(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Frame' as cValue -- field index out of range?", field);
    }
}

void FrameDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_size: pp->setSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Frame'", field);
    }
}

const char *FrameDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_blocks: return omnetpp::opp_typename(typeid(Block));
        default: return nullptr;
    };
}

omnetpp::any_ptr FrameDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        case FIELD_blocks: return omnetpp::toAnyPtr(&pp->getBlocks(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void FrameDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Frame *pp = omnetpp::fromAnyPtr<Frame>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Frame'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

