//
//  Tensors.cpp
//  QuadrocopterBrain
//
//  Created by anton on 13/02/16.
//  Copyright © 2016 anton. All rights reserved.
//

#include "Tensors.hpp"

using namespace tensorflow;

void printTensor (const Tensor& t) {
	int64 sizei = t.shape().dim_size(0);
	int64 sizej = t.shape().dim_size(1);
	auto m = t.matrix<float>();
	for (int i=0; i<sizei; i++) {
		for (int j=0; j<sizej; j++) {
			std::cerr << m (i, j) << " ";
		}
		std::cerr << std::endl;
	}
}
