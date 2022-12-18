//
// Created by JinHai on 2022/10/27.
//

#include "data_type.h"
#include "common/utility/infinity_assert.h"

#include "common/types/info/decimal_info.h"
#include "common/types/info/varchar_info.h"
#include "common/types/info/embedding_info.h"

namespace infinity {

static const char* type2name[] = {
    // Bool
    "Boolean",

    // Numeric
    "TinyInt",
    "SmallInt",
    "Integer",
    "BigInt",
    "HugeInt",
    "Float",
    "Double",
    "Decimal16",
    "Decimal32",
    "Decimal64",
    "Decimal128",

    // String
    "Varchar",
    "Char1",
    "Char2",
    "Char4",
    "Char8",
    "Char16",
    "Char32",
    "Char64",

    // Date and Time
    "Date",
    "Time",
    "DateTime",
    "Timestamp",
    "TimestampTZ",
    "Interval",

    // Nested types
    "Array",
    "Tuple",

    // Geography
    "Point",
    "Line",
    "LineSegment",
    "Box",
    "Path",
    "Polygon",
    "Circle",

    // Other
    "Bitmap",
    "UUID",
    "Blob",
    "Embedding",

    // Heterogeneous type
    "Heterogeneous",

    // only used in heterogeneous type
    "Null",
    "Missing",

    "Invalid",
};

static i64 type_size[] = {
    // Bool * 1
    1, // Boolean

    // Integer * 5
    1, // TinyInt
    2, // SmallInt
    4, // Integer
    8, // BigInt
    16, // HugeInt

    // Float * 2
    4, // Float
    8, // Double

    // Decimal * 4
    2, // Decimal16
    4, // Decimal32
    8, // Decimal64
    16, // Decimal128

    // Varchar * 1
    16, // Varchar

    // Char * 7
    1, // Char1
    2, // Char2
    4, // Char4
    8, // Char8
    16, // Char16
    32, // Char32
    64, // Char64

    // Date and Time * 6
    4, // Date
    4, // Time
    8, // DateTime
    8, // Timestamp
    8, // TimestampTZ
    8, // Interval

    // Nested types
    8, // Array
    4, // Tuple

    // Geography
    16, // Point
    24, // Line
    32, // LineSegment
    32, // Box
    16, // Path
    48, // Polygon
    24, // Circle

    // Other
    16, // Bitmap
    16, // UUID
    16, // Blob
    8, // Embedding

    // Heterogeneous
    16, // Mixed

    // only used in heterogeneous type
    0, // Null
    0, // Missing
    0, // Invalid
};

std::string
DataType::ToString() const {
    if(type_ > kInvalid) {
        TypeError("Invalid logical data type.");
    }
    return type2name[type_];
}

bool
DataType::operator==(const DataType &other) const {
    if(this == &other) return true;
    if(type_ != other.type_) return false;
    if(this->type_info_ == nullptr && other.type_info_ == nullptr) {
        return true;
    }
    if(this->type_info_ != nullptr && other.type_info_ != nullptr) {
        if(*this->type_info_ != *other.type_info_) return false;
        return true;
    } else {
        return false;
    }
}

bool
DataType::operator!=(const DataType& other) const {
    return !operator==(other);
}

size_t
DataType::Size() const {
    if(type_ > kInvalid) {
        StorageError("Invalid logical data type.");
    }

    if(type_info_ != nullptr) {
        return type_info_->Size();
    }

    return type_size[type_];
}

int64_t
DataType::CastRule(const DataType &from, const DataType &to) {
    return 0;
}

DataType
DataType::ConvertType(hsql::ColumnType type) {
    switch (type.data_type) {
        case hsql::DataType::BOOLEAN: return DataType(LogicalType::kBoolean);
        // No TinyInt
        case hsql::DataType::SMALLINT: return DataType(LogicalType::kSmallInt);
        case hsql::DataType::INT: return DataType(LogicalType::kInteger);
        case hsql::DataType::BIGINT:
        case hsql::DataType::LONG: return DataType(LogicalType::kBigInt);;

        case hsql::DataType::DECIMAL: {
            LogicalType logical_type = DecimalType::GetLogicalType(type.precision);
            SharedPtr<TypeInfo> decimal_info = DecimalInfo::Make(type.precision, type.scale);
            return DataType(logical_type, std::move(decimal_info));
        }

        case hsql::DataType::REAL:
        case hsql::DataType::FLOAT: return DataType(LogicalType::kFloat);
        case hsql::DataType::DOUBLE: return DataType(LogicalType::kDouble);

        case hsql::DataType::CHAR:
        case hsql::DataType::VARCHAR:
        case hsql::DataType::TEXT: {
            return DataType(LogicalType::kVarchar, VarcharInfo::Make(type.length));
        }

        case hsql::DataType::DATE: return DataType(LogicalType::kDate);
        case hsql::DataType::TIME: return DataType(LogicalType::kTime);
        case hsql::DataType::DATETIME: return DataType(LogicalType::kDateTime);
        case hsql::DataType::UNKNOWN: {
            TypeError("Unknown date type.");
        }
    }

    TypeError("Unexpected date type.");
}

template <> String DataType::TypeToString<BooleanT>() { return "Boolean"; }
template <> String DataType::TypeToString<TinyIntT>() { return "TinyInt"; }
template <> String DataType::TypeToString<SmallIntT>() { return "SmallInt"; }
template <> String DataType::TypeToString<IntegerT>() { return "Integer"; }
template <> String DataType::TypeToString<BigIntT>() { return "BigInt"; }
template <> String DataType::TypeToString<HugeIntT>() { return "HugeInt"; }
template <> String DataType::TypeToString<FloatT>() { return "Float"; }
template <> String DataType::TypeToString<DoubleT>() { return "Double"; }
template <> String DataType::TypeToString<Decimal16T>() { return "Decimal16"; }
template <> String DataType::TypeToString<Decimal32T>() { return "Decimal32"; }
template <> String DataType::TypeToString<Decimal64T>() { return "Decimal64"; }
template <> String DataType::TypeToString<Decimal128T>() { return "Decimal128"; }
template <> String DataType::TypeToString<VarcharT>() { return "Varchar"; }
template <> String DataType::TypeToString<Char1T>() { return "Char1"; }
template <> String DataType::TypeToString<Char2T>() { return "Char2"; }
template <> String DataType::TypeToString<Char4T>() { return "Char4"; }
template <> String DataType::TypeToString<Char8T>() { return "Char8"; }
template <> String DataType::TypeToString<Char16T>() { return "Char16"; }
template <> String DataType::TypeToString<Char32T>() { return "Char32"; }
template <> String DataType::TypeToString<Char64T>() { return "Char64"; }
template <> String DataType::TypeToString<DateT>() { return "Date"; }
template <> String DataType::TypeToString<TimeT>() { return "Time"; }
template <> String DataType::TypeToString<DateTimeT>() { return "DateTime"; }
template <> String DataType::TypeToString<TimestampT>() { return "Timestamp"; }
template <> String DataType::TypeToString<TimestampTZT>() { return "TimestampTZ"; }
template <> String DataType::TypeToString<IntervalT>() { return "Interval"; }
template <> String DataType::TypeToString<ArrayT>() { return "Array"; }
//template <> String DataType::TypeToString<TupleT>() { return "Tuple"; }
template <> String DataType::TypeToString<PointT>() { return "Point"; }
template <> String DataType::TypeToString<LineT>() { return "Line"; }
template <> String DataType::TypeToString<LineSegT>() { return "LineSegment"; }
template <> String DataType::TypeToString<BoxT>() { return "Box"; }
template <> String DataType::TypeToString<PathT>() { return "Path"; }
template <> String DataType::TypeToString<PolygonT>() { return "Polygon"; }
template <> String DataType::TypeToString<CircleT>() { return "Circle"; }
template <> String DataType::TypeToString<BitmapT>() { return "Bitmap"; }
template <> String DataType::TypeToString<UuidT>() { return "UUID"; }
template <> String DataType::TypeToString<BlobT>() { return "Blob"; }
template <> String DataType::TypeToString<EmbeddingT>() { return "Embedding"; }
template <> String DataType::TypeToString<MixedT>() { return "Heterogeneous"; }

}


