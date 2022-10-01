//
// Created by JinHai on 2022/9/16.
//

#pragma once

#include "function/scalar_function.h"
#include "function/function_set.h"
#include "storage/catalog.h"

namespace infinity {

void
RegisterPowFunction(const std::unique_ptr<Catalog> &catalog_ptr);

}