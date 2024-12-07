//
// Generated file, do not edit! Created by opp_msgtool 6.0 from Block.msg.
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
#include "Block_m.h"

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

Block::Block(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

Block::Block(const Block& other) : ::omnetpp::cMessage(other)
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
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void Block::copy(const Block& other)
{
    this->codingRate = other.codingRate;
    this->maxSize = other.maxSize;
    this->usedSize = other.usedSize;
    delete [] this->packets;
    this->packets = (other.packets_arraysize==0) ? nullptr : new TerminalPacket *[other.packets_arraysize];
    packets_arraysize = other.packets_arraysize;
    for (size_t i = 0; i < packets_arraysize; i++) {
        this->packets[i] = other.packets[i];
    }
}

void Block::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->codingRate);
    doParsimPacking(b,this->maxSize);
    doParsimPacking(b,this->usedSize);
    b->pack(packets_arraysize);
    doParsimArrayPacking(b,this->packets,packets_arraysize);
}

void Block::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->codingRate);
    doParsimUnpacking(b,this->maxSize);
    doParsimUnpacking(b,this->usedSize);
    delete [] this->packets;
    b->unpack(packets_arraysize);
    if (packets_arraysize == 0) {
        this->packets = nullptr;
    } else {
        this->packets = new TerminalPacket *[packets_arraysize];
        doParsimArrayUnpacking(b,this->packets,packets_arraysize);
    }
}

CODING_RATE Block::getCodingRate() const
{
    return this->codingRate;
}

void Block::setCodingRate(CODING_RATE codingRate)
{
    this->codingRate = codingRate;
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

size_t Block::getPacketsArraySize() const
{
    return packets_arraysize;
}

const TerminalPacket * Block::getPackets(size_t k) const
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    return this->packets[k];
}

void Block::setPacketsArraySize(size_t newSize)
{
    TerminalPacket * *packets2 = (newSize==0) ? nullptr : new TerminalPacket *[newSize];
    size_t minSize = packets_arraysize < newSize ? packets_arraysize : newSize;
    for (size_t i = 0; i < minSize; i++)
        packets2[i] = this->packets[i];
    for (size_t i = minSize; i < newSize; i++)
        packets2[i] = nullptr;
    delete [] this->packets;
    this->packets = packets2;
    packets_arraysize = newSize;
}

void Block::setPackets(size_t k, TerminalPacket * packets)
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    this->packets[k] = packets;
}

void Block::insertPackets(size_t k, TerminalPacket * packets)
{
    if (k > packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    size_t newSize = packets_arraysize + 1;
    TerminalPacket * *packets2 = new TerminalPacket *[newSize];
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

void Block::appendPackets(TerminalPacket * packets)
{
    insertPackets(packets_arraysize, packets);
}

void Block::erasePackets(size_t k)
{
    if (k >= packets_arraysize) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)packets_arraysize, (unsigned long)k);
    size_t newSize = packets_arraysize - 1;
    TerminalPacket * *packets2 = (newSize == 0) ? nullptr : new TerminalPacket *[newSize];
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
        FIELD_codingRate,
        FIELD_maxSize,
        FIELD_usedSize,
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

BlockDescriptor::BlockDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(Block)), "omnetpp::cMessage")
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
        0,    // FIELD_codingRate
        FD_ISEDITABLE,    // FIELD_maxSize
        FD_ISEDITABLE,    // FIELD_usedSize
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
        "codingRate",
        "maxSize",
        "usedSize",
        "packets",
    };
    return (field >= 0 && field < 4) ? fieldNames[field] : nullptr;
}

int BlockDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "codingRate") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "maxSize") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "usedSize") == 0) return baseIndex + 2;
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
        "CODING_RATE",    // FIELD_codingRate
        "int",    // FIELD_maxSize
        "int",    // FIELD_usedSize
        "TerminalPacket",    // FIELD_packets
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
        case FIELD_packets: { const TerminalPacket * value = pp->getPackets(i); return omnetpp::opp_typename(typeid(*value)); }
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
        case FIELD_codingRate: return enum2string(pp->getCodingRate(), "CODING_RATE");
        case FIELD_maxSize: return long2string(pp->getMaxSize());
        case FIELD_usedSize: return long2string(pp->getUsedSize());
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
        case FIELD_codingRate: return static_cast<int>(pp->getCodingRate());
        case FIELD_maxSize: return pp->getMaxSize();
        case FIELD_usedSize: return pp->getUsedSize();
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
        case FIELD_packets: pp->setPackets(i,omnetpp::fromAnyPtr<TerminalPacket>(value.pointerValue())); break;
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
        case FIELD_packets: return omnetpp::opp_typename(typeid(TerminalPacket));
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
        case FIELD_packets: pp->setPackets(i,omnetpp::fromAnyPtr<TerminalPacket>(ptr)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'Block'", field);
    }
}

namespace omnetpp {

}  // namespace omnetpp

