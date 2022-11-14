//
// Created by JinHai on 2022/10/30.
//

#pragma once

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "vector_buffer.h"
#include "bitmask.h"
#include "common/types/value.h"
#include "common/default_values.h"

namespace infinity {

enum class ColumnVectorType : i8 {
    kInvalid,
    kFlat,  // Stand without any encode
    kConstant, // All vector has same type and value
//    kDictionary, // There is a dictionary zone
//    kRLE, // Run length encoding
//    kSequence,
//    kBias,
//
//    kCollectionFlat, // May have missing
//    kCollectionDictionary, // May have missing
    kHeterogeneous, // May have missing
};

// Basic unit of column data vector
struct ColumnVector {
public:
    ColumnVectorType vector_type_ {ColumnVectorType::kInvalid};

    DataType data_type_;

    size_t capacity_{0};

    size_t tail_index_ {0};

    size_t data_type_size_{0};

    // Only a pointer to the real data in vector buffer
    ptr_t data_ptr_ {nullptr};

    // A bitmap to indicate the null information
    UniquePtr<Bitmask> nulls_ptr_;

    // this buffer is holding the data
    SharedPtr<VectorBuffer> buffer_;

    bool initialized {false};

public:
    // Construct a column vector without initialization;
    explicit
    ColumnVector(DataType data_type, ColumnVectorType vector_type = ColumnVectorType::kFlat);

    void
    Initialize(size_t capacity = DEFAULT_VECTOR_SIZE);

    [[nodiscard]] String
    ToString() const;

    // Return the <index> of the vector
    // Since it will construct a new Value object, this function shouldn't be used in vectorized computation.
    // Directly uses data_ptr in vectorized computation.
    [[nodiscard]] Value
    GetValue(idx_t index) const;

    // Set the <index> element of the vector to the specified value.
    void
    SetValue(idx_t index, const Value& Value);

    void
    AppendValue(const Value& value);

    void
    ShallowCopy(const ColumnVector &other);

    // Enlarge the column vector capacity.
    void
    Reserve(size_t new_capacity);

    void
    SetVectorType(ColumnVectorType vector_type) {
        vector_type_ = vector_type;
    }

    void
    Reset();

public:
    [[nodiscard]] const inline ColumnVectorType&
    vector_type() const {
        return vector_type_;
    }

    [[nodiscard]] const inline DataType&
    data_type() const {
        return data_type_;
    }

    [[nodiscard]] inline ptr_t data() const {
        return data_ptr_;
    }

    [[nodiscard]] inline size_t capacity() const {
        return capacity_;
    }

    [[nodiscard]] inline size_t Size() const {
        return tail_index_;
    }
};

}