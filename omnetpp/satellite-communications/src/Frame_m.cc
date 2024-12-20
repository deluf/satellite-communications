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

Register_Class(Block)

Block::Block() : ::omnetpp::cObject()
{
}

Block::Block(const Block& other) : ::omnetpp::cObject(other)
{
    copy(other);
}

Block::~Block()
{
    delete [] this->packets;
}

Block& Block::operator=(const Block& other)
{
    if (this == &other) return *this;
    ::omnetpp::cObject::operator=(other);
    copy(other);
    return *this;
}

void Block::copy(const Block& other)
{
    this->maxSize = other.maxSize;
    this->usedSize = other.usedSize;
    this->codingRate = other.codingRate;
    delete [] this->packets;
    this->packets = (other.packets_arraysize==0) ? nullptr : new Packet *[other.packets_arraysize];
    packets_arraysize = other.packets_arraysize;
    for (size_t i = 0; i < packets_arraysize; i++) {
        this->packets[i] = other.packets[i];
    }
}

void Block::parsimPack(omnetpp::cCommBuffer *b) const
{
    doParsimPacking(b,this->maxSize);
    doParsimPacking(b,this->usedSize);
    doParsimPacking(b,this->codingRate);
    b->pack(packets_arraysize);
    doParsimArrayPacking(b,this->packets,packets_arraysize);
}

void Block::parsimUnpack(omnetpp::cCommBuffer *b)
{
    doParsimUnpacking(b,this->maxSize);
    doParsimUnpacking(b,this->usedSize);
    doParsimUnpacking(b,this->codingRate);
    delete [] this->packets;
    b->unpack(packets_arraysize);
    if (packets_arraysize == 0) {
        this->packets = nullptr;
    } else {
        this->packets = new Packet *[packets_arraysize];
        doParsimArrayUnpacking(b,this->packets,packets_arraysize);
    }
}

int Block::getMaxSize() const
{
    return this->maxSize;
}

void Block::setMaxSize(int maxSize)
{
    this->maxSize = maxSize;
}

int Block::getUsedSize() const
{
    return this->usedSize;
}

void Block::setUsedSize(int usedSize)
{
    this->usedSize = usedSize;
}

CODING_RATE Block::getCodingRate() const
{
    return this->codingRate;
}

void Block::setCodingRate(CODING_RATE codingRate)
{
    this->codingRate = codingRate;
}

size_t Block::getPacketsArraySize() const
{
    return packets_arraysize;
}

const Packet * Block::getPackets(size_t k) const
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    return this->packets[k];
}

void Block::setPacketsArraySize(size_t newSize)
{
    Packet * *packets2 = (newSize==0) ? nullptr : new Packet *[newSize];
    size_t minSize = packets_arraysize < newSize ? packets_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        packets2[i] = this->packets[i];
    for (size_t i = minSize; i < newSize; i++)
        packets2[i] = nullptr;
    delete [] this->packets;
    this->packets = packets2;
    packets_arraysize = newSize;
}

void Block::setPackets(size_t k, Packet * packets)
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    this->packets[k] = packets;
}

void Block::insertPackets(size_t k, Packet * packets)
{
    if (k > packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    size_t newSize = packets_arraysize + 1;
    Packet * *packets2 = new Packet *[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        packets2[i] = this->packets[i];
    packets2[k] = packets;
    for (i = k + 1; i < newSize; i++)
        packets2[i] = this->packets[i-1];
    delete [] this->packets;
    this->packets = packets2;
    packets_arraysize = newSize;
}

void Block::appendPackets(Packet * packets)
{
    insertPackets(packets_arraysize, packets);
}

void Block::erasePackets(size_t k)
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    size_t newSize = packets_arraysize - 1;
    Packet * *packets2 = (newSize == 0) ? nullptr : new Packet *[newSize];
    size_t i;
    for (i = 0; i < k; i++)
        packets2[i] = this->packets[i];
    for (i = k; i < newSize; i++)
        packets2[i] = this->packets[i+1];
    delete [] this->packets;
    this->packets = packets2;
    packets_arraysize = newSize;
}

class BlockDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_maxSize,
        FIELD_usedSize,
        FIELD_codingRate,
        FIELD_packets,
    };
  public:
    BlockDescriptor();
    virtual ~BlockDescriptor();

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

Register_ClassDescriptor(BlockDescriptor)

BlockDescriptor::BlockDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Block)), "omnetpp::cObject")
{
    propertyNames = nullptr;
}

BlockDescriptor::~BlockDescriptor()
{
    delete[] propertyNames;
}

bool BlockDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<Block *>(obj)!=nullptr;
}

const char **BlockDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *BlockDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int BlockDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 4+base->getFieldCount() : 4;
}

unsigned int BlockDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_maxSize
        FD_ISEDITABLE,    // FIELD_usedSize
        FD_ISEDITABLE,    // FIELD_codingRate
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISPOINTER | FD_ISCOBJECT | FD_ISCOWNEDOBJECT | FD_ISREPLACEABLE | FD_ISRESIZABLE,    // FIELD_packets
    };
    return (field >= 0 && field < 4) ? fieldTypeFlags[field] : 0;
}

const char *BlockDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "maxSize",
        "usedSize",
        "codingRate",
        "packets",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int BlockDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "maxSize") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "usedSize") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "codingRate") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "packets") == 0) return baseIndex + 3;
    return base ? base->findField(fieldName) : -1;
}

const char *BlockDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_maxSize
        "int",    // FIELD_usedSize
        "CODING_RATE",    // FIELD_codingRate
        "Packet",    // FIELD_packets
    };
    return (field >= 0 && field < 4) ? fieldTypeStrings[field] : nullptr;
}

const char **BlockDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_codingRate: {
            static const char *names[] = { "enum",  nullptr };
            return names;
        }
        default: return nullptr;
    }
}

const char *BlockDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_codingRate:
            if (!strcmp(propertyName, "enum")) return "CODING_RATE";
            return nullptr;
        default: return nullptr;
    }
}

int BlockDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_packets: return pp->getPacketsArraySize();
        default: return 0;
    }
}

void BlockDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_packets: pp->setPacketsArraySize(size); break;
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'Block'", field);
    }
}

const char *BlockDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_packets: { const Packet * value = pp->getPackets(i); return omnetpp::opp_typename(typeid(*value)); }
        default: return nullptr;
    }
}

std::string BlockDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_maxSize: return long2string(pp->getMaxSize());
        case FIELD_usedSize: return long2string(pp->getUsedSize());
        case FIELD_codingRate: return enum2string(pp->getCodingRate(), "CODING_RATE");
        case FIELD_packets: { auto obj = pp->getPackets(i); return obj == nullptr ? "" : obj->str(); }
        default: return "";
    }
}

void BlockDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_maxSize: pp->setMaxSize(string2long(value)); break;
        case FIELD_usedSize: pp->setUsedSize(string2long(value)); break;
        case FIELD_codingRate: pp->setCodingRate((CODING_RATE)string2enum(value, "CODING_RATE")); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Block'", field);
    }
}

omnetpp::cValue BlockDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_maxSize: return pp->getMaxSize();
        case FIELD_usedSize: return pp->getUsedSize();
        case FIELD_codingRate: return static_cast<int>(pp->getCodingRate());
        case FIELD_packets: return omnetpp::toAnyPtr(pp->getPackets(i)); break;
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'Block' as cValue -- field index out of range?", field);
    }
}

void BlockDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_maxSize: pp->setMaxSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_usedSize: pp->setUsedSize(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_codingRate: pp->setCodingRate(static_cast<CODING_RATE>(value.intValue())); break;
        case FIELD_packets: pp->setPackets(i,omnetpp::fromAnyPtr<Packet>(value.pointerValue())); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Block'", field);
    }
}

const char *BlockDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        case FIELD_packets: return omnetpp::opp_typename(typeid(Packet));
        default: return nullptr;
    };
}

omnetpp::any_ptr BlockDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_packets: return omnetpp::toAnyPtr(pp->getPackets(i)); break;
        default: return omnetpp::any_ptr(nullptr);
    }
}

void BlockDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    Block *pp = omnetpp::fromAnyPtr<Block>(object); (void)pp;
    switch (field) {
        case FIELD_packets: pp->setPackets(i,omnetpp::fromAnyPtr<Packet>(ptr)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Block'", field);
    }
}

Register_Class(Frame)

Frame::Frame(const char *name, short kind) : ::omnetpp::cPacket(name, kind)
{
}

Frame::Frame(const Frame& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

Frame::~Frame()
{
    delete [] this->blocks;
}

Frame& Frame::operator=(const Frame& other)
{
    if (this == &other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void Frame::copy(const Frame& other)
{
    delete [] this->blocks;
    this->blocks = (other.blocks_arraysize==0) ? nullptr : new Block[other.blocks_arraysize];
    blocks_arraysize = other.blocks_arraysize;
    for (size_t i = 0; i < blocks_arraysize; i++) {
        this->blocks[i] = other.blocks[i];
    }
}

void Frame::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    b->pack(blocks_arraysize);
    doParsimArrayPacking(b,this->blocks,blocks_arraysize);
}

void Frame::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    delete [] this->blocks;
    b->unpack(blocks_arraysize);
    if (blocks_arraysize == 0) {
        this->blocks = nullptr;
    } else {
        this->blocks = new Block[blocks_arraysize];
        doParsimArrayUnpacking(b,this->blocks,blocks_arraysize);
    }
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
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
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
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
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
    delete [] this->blocks;
    this->blocks = blocks2;
    blocks_arraysize = newSize;
}

class FrameDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
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

FrameDescriptor::FrameDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Frame)), "omnetpp::cPacket")
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
    return base ? 1+base->getFieldCount() : 1;
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
        FD_ISARRAY | FD_ISCOMPOUND | FD_ISCOBJECT | FD_ISRESIZABLE,    // FIELD_blocks
    };
    return (field >= 0 && field < 1) ? fieldTypeFlags[field] : 0;
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
        "blocks",
    };
    return (field >= 0 && field < 1) ? fieldNames[field] : nullptr;
}

int FrameDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "blocks") == 0) return baseIndex + 0;
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
        "Block",    // FIELD_blocks
    };
    return (field >= 0 && field < 1) ? fieldTypeStrings[field] : nullptr;
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

