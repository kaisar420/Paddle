/* Copyright (c) 2016 PaddlePaddle Authors. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License. */

#include "paddle/framework/lod_rank_table.h"
#include "paddle/framework/op_registry.h"
#include "paddle/framework/operator.h"

namespace paddle {
namespace operators {

class MaxSeqenceLenOp : public framework::OperatorBase {
 public:
  MaxSeqenceLenOp(const std::string &type,
                  const framework::VariableNameMap &inputs,
                  const framework::VariableNameMap &outputs,
                  const framework::AttributeMap &attrs)
      : OperatorBase(type, inputs, outputs, attrs) {}

  void Run(const framework::Scope &scope,
           const platform::Place &dev_place) const override {
    auto &rank_table =
        scope.FindVar(Input("RankTable"))->Get<framework::LoDRankTable>();
    auto *out =
        scope.FindVar(Output("Out"))->GetMutable<framework::LoDTensor>();
    int64_t *out_ptr = out->mutable_data<int64_t>({1}, platform::CPUPlace());
    *out_ptr = rank_table.items()[0].length;
  }
};

class MaxSeqenceLenOpProtoMaker : public framework::OpProtoAndCheckerMaker {
 public:
  MaxSeqenceLenOpProtoMaker(OpProto *proto, OpAttrChecker *op_checker)
      : OpProtoAndCheckerMaker(proto, op_checker) {
    AddInput("RankTable", "The lod_rank_table.");
    AddOutput("Out", "The max sequence length.");
    AddComment(
        R"DOC(Calculate the max sequence length through lod_rank_table.)DOC");
  }
};

class MaxSeqenceLenInferShape : public framework::InferShapeBase {
 public:
  void operator()(framework::InferShapeContext *context) const override {
    PADDLE_ENFORCE(context->HasInput("RankTable"));
    context->SetOutputDim("Out", {1});
  }
};
}  // namespace operators
}  // namespace paddle

REGISTER_OPERATOR(max_sequence_len, paddle::operators::MaxSeqenceLenOp,
                  paddle::operators::MaxSeqenceLenOpProtoMaker,
                  paddle::operators::MaxSeqenceLenInferShape,
                  paddle::framework::EmptyGradOpMaker);
