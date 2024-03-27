#include "geometryData.h"
#include <glad/glad.h>
#include "util/check.h"
GeometryData::GeometryData(const std::vector<float>& data) {
  data_ = data;
  initData();
}
void GeometryData::initData() {
  // configure VAO/VBO
  unsigned int VBO;
  glGenVertexArrays(1, &this->geometryVAO_);
  glGenBuffers(1, &VBO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data_.size(), data_.data(),
               GL_STATIC_DRAW);

  glBindVertexArray(this->geometryVAO_);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}