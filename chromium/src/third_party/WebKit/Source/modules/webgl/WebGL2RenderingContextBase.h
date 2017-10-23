// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGL2RenderingContextBase_h
#define WebGL2RenderingContextBase_h

#include "bindings/core/v8/TraceWrapperMember.h"
#include "modules/webgl/WebGLExtension.h"
#include "modules/webgl/WebGLRenderingContextBase.h"
#include <memory>

namespace blink {

class WebGLTexture;

class WebGLActiveInfo;
class WebGLBuffer;
class WebGLGetBufferSubDataAsync;
class WebGLGetBufferSubDataAsyncCallback;
class WebGLProgram;
class WebGLQuery;
class WebGLSampler;
class WebGLSync;
class WebGLTransformFeedback;
class WebGLUniformLocation;
class WebGLVertexArrayObject;

class WebGL2RenderingContextBase : public WebGLRenderingContextBase {
 public:
  void destroyContext() override;

  /* Buffer objects */
  void bufferData(GLenum, DOMArrayBufferView*, GLenum, GLuint, GLuint);
  void bufferSubData(GLenum, GLintptr, DOMArrayBufferView*, GLuint, GLuint);
  // Have to re-declare/re-define the following buffer{Sub}Data functions from
  // base class.  This is because the above buffer{Sub}Data() hides the name
  // from base class.
  void bufferData(GLenum target, long long size, GLenum usage);
  void bufferData(GLenum target, DOMArrayBuffer* data, GLenum usage);
  void bufferData(GLenum target, DOMArrayBufferView* data, GLenum usage);
  void bufferSubData(GLenum target, long long offset, DOMArrayBuffer* data);
  void bufferSubData(GLenum target,
                     long long offset,
                     const FlexibleArrayBufferView& data);

  void copyBufferSubData(GLenum, GLenum, long long, long long, long long);
  void getBufferSubData(GLenum, long long, DOMArrayBufferView*, GLuint, GLuint);

  void registerGetBufferSubDataAsyncCallback(
      WebGLGetBufferSubDataAsyncCallback*);
  void unregisterGetBufferSubDataAsyncCallback(
      WebGLGetBufferSubDataAsyncCallback*);

  /* Framebuffer objects */
  bool validateTexFuncLayer(const char*, GLenum texTarget, GLint layer);
  void blitFramebuffer(GLint,
                       GLint,
                       GLint,
                       GLint,
                       GLint,
                       GLint,
                       GLint,
                       GLint,
                       GLbitfield,
                       GLenum);
  void framebufferTextureLayer(GLenum, GLenum, WebGLTexture*, GLint, GLint);
  ScriptValue getInternalformatParameter(ScriptState*, GLenum, GLenum, GLenum);
  void invalidateFramebuffer(GLenum, const Vector<GLenum>&);
  void invalidateSubFramebuffer(GLenum,
                                const Vector<GLenum>&,
                                GLint,
                                GLint,
                                GLsizei,
                                GLsizei);
  void readBuffer(GLenum);

  /* Renderbuffer objects */
  void renderbufferStorageMultisample(GLenum,
                                      GLsizei,
                                      GLenum,
                                      GLsizei,
                                      GLsizei);

  /* Texture objects */
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  GLintptr);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  ImageData*);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLImageElement*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLCanvasElement*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLVideoElement*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  ImageBitmap*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  DOMArrayBufferView*,
                  GLuint);

  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     GLintptr);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     ImageData*);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLImageElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLCanvasElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLVideoElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     ImageBitmap*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     DOMArrayBufferView*,
                     GLuint);

  // Have to re-declare/re-define the following tex{Sub}Image2D functions from
  // base class.  This is because the above tex{Sub}Image2D() hides the name
  // from base class.
  void texImage2D(GLenum, GLint, GLint, GLenum, GLenum, ImageData*);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLImageElement*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLCanvasElement*,
                  ExceptionState&);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLVideoElement*,
                  ExceptionState&);
  void texImage2D(GLenum target, 
                  GLint level, 
                  GLint internalformat,
                  GLenum format, 
                  GLenum type, 
                  VRPassThroughCamera*);
  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLenum,
                  GLenum,
                  ImageBitmap*,
                  ExceptionState&);
  void texSubImage2D(GLenum, GLint, GLint, GLint, GLenum, GLenum, ImageData*);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLenum,
                     GLenum,
                     HTMLImageElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLenum,
                     GLenum,
                     HTMLCanvasElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLenum,
                     GLenum,
                     HTMLVideoElement*,
                     ExceptionState&);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLenum,
                     GLenum,
                     ImageBitmap*,
                     ExceptionState&);

  void texStorage2D(GLenum, GLsizei, GLenum, GLsizei, GLsizei);
  void texStorage3D(GLenum, GLsizei, GLenum, GLsizei, GLsizei, GLsizei);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  DOMArrayBufferView*);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  DOMArrayBufferView*,
                  GLuint);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  ImageData*);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLImageElement*,
                  ExceptionState&);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLCanvasElement*,
                  ExceptionState&);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  HTMLVideoElement*,
                  ExceptionState&);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  ImageBitmap*,
                  ExceptionState&);
  void texImage3D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  GLintptr);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     DOMArrayBufferView*,
                     GLuint);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     GLintptr);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     ImageData*);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLImageElement*,
                     ExceptionState&);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLCanvasElement*,
                     ExceptionState&);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     HTMLVideoElement*,
                     ExceptionState&);
  void texSubImage3D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     ImageBitmap*,
                     ExceptionState&);

  void texImage2D(GLenum,
                  GLint,
                  GLint,
                  GLsizei,
                  GLsizei,
                  GLint,
                  GLenum,
                  GLenum,
                  DOMArrayBufferView*);
  void texSubImage2D(GLenum,
                     GLint,
                     GLint,
                     GLint,
                     GLsizei,
                     GLsizei,
                     GLenum,
                     GLenum,
                     DOMArrayBufferView*);

  void copyTexSubImage3D(GLenum,
                         GLint,
                         GLint,
                         GLint,
                         GLint,
                         GLint,
                         GLint,
                         GLsizei,
                         GLsizei);

  void compressedTexImage2D(GLenum target,
                            GLint level,
                            GLenum internalformat,
                            GLsizei width,
                            GLsizei height,
                            GLint border,
                            DOMArrayBufferView* data,
                            GLuint srcOffset,
                            GLuint srcLengthOverride);
  void compressedTexSubImage2D(GLenum target,
                               GLint level,
                               GLint xoffset,
                               GLint yoffset,
                               GLsizei width,
                               GLsizei height,
                               GLenum format,
                               DOMArrayBufferView* data,
                               GLuint srcOffset,
                               GLuint srcLengthOverride);
  void compressedTexImage3D(GLenum,
                            GLint,
                            GLenum,
                            GLsizei,
                            GLsizei,
                            GLsizei,
                            GLint,
                            DOMArrayBufferView*,
                            GLuint,
                            GLuint);
  void compressedTexSubImage3D(GLenum,
                               GLint,
                               GLint,
                               GLint,
                               GLint,
                               GLsizei,
                               GLsizei,
                               GLsizei,
                               GLenum,
                               DOMArrayBufferView*,
                               GLuint,
                               GLuint);
  void compressedTexImage2D(GLenum target,
                            GLint level,
                            GLenum internalformat,
                            GLsizei width,
                            GLsizei height,
                            GLint border,
                            GLsizei imageSize,
                            GLintptr offset);
  void compressedTexSubImage2D(GLenum target,
                               GLint level,
                               GLint xoffset,
                               GLint yoffset,
                               GLsizei width,
                               GLsizei height,
                               GLenum format,
                               GLsizei imageSize,
                               GLintptr offset);
  void compressedTexImage3D(GLenum target,
                            GLint level,
                            GLenum internalformat,
                            GLsizei width,
                            GLsizei height,
                            GLsizei depth,
                            GLint border,
                            GLsizei imageSize,
                            GLintptr offset);
  void compressedTexSubImage3D(GLenum target,
                               GLint level,
                               GLint xoffset,
                               GLint yoffset,
                               GLint zoffset,
                               GLsizei width,
                               GLsizei height,
                               GLsizei depth,
                               GLenum format,
                               GLsizei imageSize,
                               GLintptr offset);

  // Have to re-declare/re-define the following compressedTex{Sub}Image2D
  // functions from the base class. This is because the above
  // compressedTex{Sub}Image2D() hide the name from base class.
  void compressedTexImage2D(GLenum target,
                            GLint level,
                            GLenum internalformat,
                            GLsizei width,
                            GLsizei height,
                            GLint border,
                            DOMArrayBufferView* data);
  void compressedTexSubImage2D(GLenum target,
                               GLint level,
                               GLint xoffset,
                               GLint yoffset,
                               GLsizei width,
                               GLsizei height,
                               GLenum format,
                               DOMArrayBufferView* data);

  /* Programs and shaders */
  GLint getFragDataLocation(WebGLProgram*, const String&);

  /* Uniforms and attributes */
  void uniform1ui(const WebGLUniformLocation*, GLuint);
  void uniform2ui(const WebGLUniformLocation*, GLuint, GLuint);
  void uniform3ui(const WebGLUniformLocation*, GLuint, GLuint, GLuint);
  void uniform4ui(const WebGLUniformLocation*, GLuint, GLuint, GLuint, GLuint);
  void uniform1uiv(const WebGLUniformLocation*, const FlexibleUint32ArrayView&);
  void uniform1uiv(const WebGLUniformLocation*, Vector<GLuint>&);
  void uniform2uiv(const WebGLUniformLocation*, const FlexibleUint32ArrayView&);
  void uniform2uiv(const WebGLUniformLocation*, Vector<GLuint>&);
  void uniform3uiv(const WebGLUniformLocation*, const FlexibleUint32ArrayView&);
  void uniform3uiv(const WebGLUniformLocation*, Vector<GLuint>&);
  void uniform4uiv(const WebGLUniformLocation*, const FlexibleUint32ArrayView&);
  void uniform4uiv(const WebGLUniformLocation*, Vector<GLuint>&);
  void uniformMatrix2x3fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix2x3fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);
  void uniformMatrix3x2fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix3x2fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);
  void uniformMatrix2x4fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix2x4fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);
  void uniformMatrix4x2fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix4x2fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);
  void uniformMatrix3x4fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix3x4fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);
  void uniformMatrix4x3fv(const WebGLUniformLocation*,
                          GLboolean,
                          DOMFloat32Array*);
  void uniformMatrix4x3fv(const WebGLUniformLocation*,
                          GLboolean,
                          Vector<GLfloat>&);

  void vertexAttribI4i(GLuint, GLint, GLint, GLint, GLint);
  void vertexAttribI4iv(GLuint, const DOMInt32Array*);
  void vertexAttribI4iv(GLuint, const Vector<GLint>&);
  void vertexAttribI4ui(GLuint, GLuint, GLuint, GLuint, GLuint);
  void vertexAttribI4uiv(GLuint, const DOMUint32Array*);
  void vertexAttribI4uiv(GLuint, const Vector<GLuint>&);
  void vertexAttribIPointer(GLuint index,
                            GLint size,
                            GLenum type,
                            GLsizei stride,
                            long long offset);

  /* Writing to the drawing buffer */
  void vertexAttribDivisor(GLuint, GLuint);
  void drawArraysInstanced(GLenum, GLint, GLsizei, GLsizei);
  void drawElementsInstanced(GLenum, GLsizei, GLenum, long long, GLsizei);
  void drawRangeElements(GLenum mode,
                         GLuint start,
                         GLuint end,
                         GLsizei count,
                         GLenum type,
                         long long offset);

  /* Multiple Render Targets */
  void drawBuffers(const Vector<GLenum>&);
  void clearBufferiv(GLenum, GLint, DOMInt32Array*);
  void clearBufferiv(GLenum, GLint, const Vector<GLint>&);
  void clearBufferuiv(GLenum, GLint, DOMUint32Array*);
  void clearBufferuiv(GLenum, GLint, const Vector<GLuint>&);
  void clearBufferfv(GLenum, GLint, DOMFloat32Array*);
  void clearBufferfv(GLenum, GLint, const Vector<GLfloat>&);
  void clearBufferfi(GLenum, GLint, GLfloat, GLint);

  /* Query Objects */
  WebGLQuery* createQuery();
  void deleteQuery(WebGLQuery*);
  GLboolean isQuery(WebGLQuery*);
  void beginQuery(GLenum, WebGLQuery*);
  void endQuery(GLenum);
  ScriptValue getQuery(ScriptState*, GLenum, GLenum);
  ScriptValue getQueryParameter(ScriptState*, WebGLQuery*, GLenum);

  /* Sampler Objects */
  WebGLSampler* createSampler();
  void deleteSampler(WebGLSampler*);
  GLboolean isSampler(WebGLSampler*);
  void bindSampler(GLuint, WebGLSampler*);
  void samplerParameteri(WebGLSampler*, GLenum, GLint);
  void samplerParameterf(WebGLSampler*, GLenum, GLfloat);
  ScriptValue getSamplerParameter(ScriptState*, WebGLSampler*, GLenum);

  /* Sync objects */
  WebGLSync* fenceSync(GLenum, GLbitfield);
  GLboolean isSync(WebGLSync*);
  void deleteSync(WebGLSync*);
  GLenum clientWaitSync(WebGLSync*, GLbitfield, GLuint64);
  void waitSync(WebGLSync*, GLbitfield, GLint64);

  ScriptValue getSyncParameter(ScriptState*, WebGLSync*, GLenum);

  /* Transform Feedback */
  WebGLTransformFeedback* createTransformFeedback();
  void deleteTransformFeedback(WebGLTransformFeedback*);
  GLboolean isTransformFeedback(WebGLTransformFeedback*);
  void bindTransformFeedback(GLenum, WebGLTransformFeedback*);
  void beginTransformFeedback(GLenum);
  void endTransformFeedback();
  void transformFeedbackVaryings(WebGLProgram*, const Vector<String>&, GLenum);
  WebGLActiveInfo* getTransformFeedbackVarying(WebGLProgram*, GLuint);
  void pauseTransformFeedback();
  void resumeTransformFeedback();
  bool validateTransformFeedbackPrimitiveMode(const char* functionName,
                                              GLenum primitiveMode);

  /* Uniform Buffer Objects and Transform Feedback Buffers */
  void bindBufferBase(GLenum, GLuint, WebGLBuffer*);
  void bindBufferRange(GLenum, GLuint, WebGLBuffer*, long long, long long);
  ScriptValue getIndexedParameter(ScriptState*, GLenum, GLuint);
  Vector<GLuint> getUniformIndices(WebGLProgram*, const Vector<String>&);
  ScriptValue getActiveUniforms(ScriptState*,
                                WebGLProgram*,
                                const Vector<GLuint>&,
                                GLenum);
  GLuint getUniformBlockIndex(WebGLProgram*, const String&);
  ScriptValue getActiveUniformBlockParameter(ScriptState*,
                                             WebGLProgram*,
                                             GLuint,
                                             GLenum);
  String getActiveUniformBlockName(WebGLProgram*, GLuint);
  void uniformBlockBinding(WebGLProgram*, GLuint, GLuint);

  /* Vertex Array Objects */
  WebGLVertexArrayObject* createVertexArray();
  void deleteVertexArray(WebGLVertexArrayObject*);
  GLboolean isVertexArray(WebGLVertexArrayObject*);
  void bindVertexArray(WebGLVertexArrayObject*);

  /* Reading */
  void readPixels(GLint x,
                  GLint y,
                  GLsizei width,
                  GLsizei height,
                  GLenum format,
                  GLenum type,
                  DOMArrayBufferView* pixels,
                  GLuint offset);
  void readPixels(GLint x,
                  GLint y,
                  GLsizei width,
                  GLsizei height,
                  GLenum format,
                  GLenum type,
                  long long offset);

  /* WebGLRenderingContextBase overrides */
  void initializeNewContext() override;
  void bindFramebuffer(GLenum target, WebGLFramebuffer*) override;
  void deleteFramebuffer(WebGLFramebuffer*) override;
  ScriptValue getParameter(ScriptState*, GLenum pname) override;
  ScriptValue getTexParameter(ScriptState*,
                              GLenum target,
                              GLenum pname) override;
  ScriptValue getFramebufferAttachmentParameter(ScriptState*,
                                                GLenum target,
                                                GLenum attachment,
                                                GLenum pname) override;
  void pixelStorei(GLenum pname, GLint param) override;
  void readPixels(GLint x,
                  GLint y,
                  GLsizei width,
                  GLsizei height,
                  GLenum format,
                  GLenum type,
                  DOMArrayBufferView* pixels) override;
  void restoreCurrentFramebuffer() override;

  DECLARE_VIRTUAL_TRACE();
  DECLARE_VIRTUAL_TRACE_WRAPPERS();

 protected:
  friend class V8WebGL2RenderingContext;
  friend class WebGLGetBufferSubDataAsync;

  WebGL2RenderingContextBase(
      HTMLCanvasElement*,
      std::unique_ptr<WebGraphicsContext3DProvider>,
      const CanvasContextCreationAttributes& requestedAttributes);
  WebGL2RenderingContextBase(
      OffscreenCanvas*,
      std::unique_ptr<WebGraphicsContext3DProvider>,
      const CanvasContextCreationAttributes& requestedAttributes);

  // DrawingBuffer::Client implementation.
  void DrawingBufferClientRestorePixelUnpackBufferBinding() override;

  // Helper function to validate target and the attachment combination for
  // getFramebufferAttachmentParameters.  Generate GL error and return false if
  // parameters are illegal.
  bool validateGetFramebufferAttachmentParameterFunc(const char* functionName,
                                                     GLenum target,
                                                     GLenum attachment);

  bool validateClearBuffer(const char* functionName,
                           GLenum buffer,
                           GLsizei length);

  enum TexStorageType {
    TexStorageType2D,
    TexStorageType3D,
  };
  bool validateTexStorage(const char*,
                          GLenum,
                          GLsizei,
                          GLenum,
                          GLsizei,
                          GLsizei,
                          GLsizei,
                          TexStorageType);

  bool validateUniformBlockIndex(const char*, WebGLProgram*, GLuint);

  ScriptValue getInt64Parameter(ScriptState*, GLenum);

  void samplerParameter(WebGLSampler*, GLenum, GLfloat, GLint, bool);

  bool isBufferBoundToTransformFeedback(WebGLBuffer*);
  bool isBufferBoundToNonTransformFeedback(WebGLBuffer*);
  bool validateBufferTargetCompatibility(const char*, GLenum, WebGLBuffer*);

  bool validateBufferBaseTarget(const char* functionName, GLenum target);
  bool validateAndUpdateBufferBindBaseTarget(const char* functionName,
                                             GLenum,
                                             GLuint,
                                             WebGLBuffer*);

  WebGLImageConversion::PixelStoreParams getPackPixelStoreParams() override;
  WebGLImageConversion::PixelStoreParams getUnpackPixelStoreParams(
      TexImageDimension) override;

  bool checkAndTranslateAttachments(const char* functionName,
                                    GLenum,
                                    Vector<GLenum>&);

  IntRect getTextureSourceSubRectangle(GLsizei width, GLsizei height);

  /* WebGLRenderingContextBase overrides */
  unsigned getMaxWebGLLocationLength() const override { return 1024; };
  bool validateCapability(const char* functionName, GLenum) override;
  bool validateBufferTarget(const char* functionName, GLenum target) override;
  bool validateAndUpdateBufferBindTarget(const char* functionName,
                                         GLenum,
                                         WebGLBuffer*) override;
  bool validateFramebufferTarget(GLenum target) override;

  bool validateReadPixelsFormatAndType(GLenum format,
                                       GLenum type,
                                       DOMArrayBufferView*) override;
  WebGLFramebuffer* getFramebufferBinding(GLenum target) override;
  WebGLFramebuffer* getReadFramebufferBinding() override;
  GLint getMaxTextureLevelForTarget(GLenum target) override;
  void renderbufferStorageImpl(GLenum target,
                               GLsizei samples,
                               GLenum internalformat,
                               GLsizei width,
                               GLsizei height,
                               const char* functionName) override;

  WebGLTexture* validateTexImageBinding(const char*,
                                        TexImageFunctionID,
                                        GLenum) override;
  // Helper function to check texture 3D target and texture bound to the target.
  // Generate GL errors and return 0 if target is invalid or texture bound is
  // null.  Otherwise, return the texture bound to the target.
  WebGLTexture* validateTexture3DBinding(const char* functionName,
                                         GLenum target);

  WebGLBuffer* validateBufferDataTarget(const char* functionName,
                                        GLenum target) override;
  bool validateBufferDataUsage(const char* functionName, GLenum usage) override;

  const char* validateGetBufferSubData(const char* functionName,
                                       GLenum target,
                                       GLintptr sourceByteOffset,
                                       DOMArrayBufferView*,
                                       GLuint destinationOffset,
                                       GLuint length,
                                       WebGLBuffer**,
                                       void** outDestinationDataPtr,
                                       long long* outDestinationByteLength);
  const char* validateGetBufferSubDataBounds(const char* functionName,
                                             WebGLBuffer*,
                                             GLintptr sourceByteOffset,
                                             long long destinationByteLength);

  bool canUseTexImageByGPU(TexImageFunctionID,
                           GLint internalformat,
                           GLenum type) override;

  void removeBoundBuffer(WebGLBuffer*) override;

  void resetUnpackParameters() override;
  void restoreUnpackParameters() override;

  void visitChildDOMWrappers(v8::Isolate*,
                             const v8::Persistent<v8::Object>&) override;

  void renderbufferStorageHelper(GLenum target,
                                 GLsizei samples,
                                 GLenum internalformat,
                                 GLsizei width,
                                 GLsizei height,
                                 const char* functionName);

  TraceWrapperMember<WebGLFramebuffer> m_readFramebufferBinding;
  TraceWrapperMember<WebGLTransformFeedback> m_transformFeedbackBinding;

  std::set<GLenum> m_supportedInternalFormatsStorage;
  std::set<GLenum> m_compressedTextureFormatsETC2EAC;

  TraceWrapperMember<WebGLBuffer> m_boundCopyReadBuffer;
  TraceWrapperMember<WebGLBuffer> m_boundCopyWriteBuffer;
  TraceWrapperMember<WebGLBuffer> m_boundPixelPackBuffer;
  TraceWrapperMember<WebGLBuffer> m_boundPixelUnpackBuffer;
  TraceWrapperMember<WebGLBuffer> m_boundTransformFeedbackBuffer;
  TraceWrapperMember<WebGLBuffer> m_boundUniformBuffer;

  HeapVector<TraceWrapperMember<WebGLBuffer>>
      m_boundIndexedTransformFeedbackBuffers;
  HeapVector<TraceWrapperMember<WebGLBuffer>> m_boundIndexedUniformBuffers;
  GLint m_maxTransformFeedbackSeparateAttribs;
  size_t m_maxBoundUniformBufferIndex;

  TraceWrapperMember<WebGLQuery> m_currentBooleanOcclusionQuery;
  TraceWrapperMember<WebGLQuery>
      m_currentTransformFeedbackPrimitivesWrittenQuery;
  TraceWrapperMember<WebGLQuery> m_currentElapsedQuery;
  HeapVector<TraceWrapperMember<WebGLSampler>> m_samplerUnits;

  GLint m_packRowLength;
  GLint m_packSkipPixels;
  GLint m_packSkipRows;
  GLint m_unpackRowLength;
  GLint m_unpackImageHeight;
  GLint m_unpackSkipPixels;
  GLint m_unpackSkipRows;
  GLint m_unpackSkipImages;

  HeapHashSet<Member<WebGLGetBufferSubDataAsyncCallback>>
      m_getBufferSubDataAsyncCallbacks;
};

DEFINE_TYPE_CASTS(WebGL2RenderingContextBase,
                  CanvasRenderingContext,
                  context,
                  context->is3d() &&
                      WebGLRenderingContextBase::getWebGLVersion(context) >= 2,
                  context.is3d() &&
                      WebGLRenderingContextBase::getWebGLVersion(&context) >=
                          2);

}  // namespace blink

#endif
