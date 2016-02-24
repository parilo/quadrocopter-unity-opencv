//
//  Tensors.hpp
//  QuadrocopterBrain
//
//  Created by anton on 13/02/16.
//  Copyright © 2016 anton. All rights reserved.
//

#ifndef Tensors_hpp
#define Tensors_hpp

#include "tensorflow/core/public/session.h"
#include "tensorflow/core/public/env.h"

void printTensor (const tensorflow::Tensor& t);

#endif /* Tensors_hpp */
