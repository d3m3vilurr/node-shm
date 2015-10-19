#if !defined BUILDING_NODE_EXTENSION
	#define BUILDING_NODE_EXTENSION
#endif
#include <node.h>
#include <nan.h>
#include <v8.h>
#include "node_buffer.h"
#include "SHMobject.h"

using namespace v8;

#define NanReturn(value) { info.GetReturnValue().Set(value); return; }
#define NanException(type, msg) Exception::type(Nan::New(msg).ToLocalChecked())
#define NanThrowException(exc) { Nan::ThrowError(exc); NanReturn(Nan::Undefined()); }

NAN_METHOD(GetErrMsg) {
  Nan::HandleScope scope;
	int activateDebug = info[0]->Int32Value();	//IntegerValue();
	shm_setDbg(activateDebug);
	Local<Value> b =  Encode(shm_err(), 1024, node::BINARY);
  NanReturn(b);
}

//API:
//	int shmid  .... int shmop_open (int key, char* flags, int mode, int size)
//	unsigned char*  shmop_read (int shmid, int start, int length)
//	int  shmop_close (int shmid)
//	int  shmop_size (int shmid)
//	int  shmop_write (int shmid, char * data, int offset, int data_len)
//	int shmop_delete (int shmid)
//

NAN_METHOD(ShmOpen) {
//	int shmid  .... int shmop_open (int key, char* flags, int mode, int size)
  Nan::HandleScope scope;
  char *flags = NULL;
  int key = info[0]->Int32Value();	//IntegerValue();
  String::Utf8Value sflags(info[1]->ToString());;
  flags = new char[(sflags.length()+1)];
  memcpy(flags,*sflags,sflags.length()+1);
  int mode = info[2]->Int32Value();	//IntegerValue();
  int size = info[3]->Int32Value();	//IntegerValue();

  int retval = shmop_open (key, flags, mode, size);
  NanReturn(Nan::New<Number>(retval));
}

NAN_METHOD(ShmRead) {
//	unsigned char*  shmop_read (int shmid, int start, int length)
  Nan::HandleScope scope;
  char *sdata;
  int shmid = info[0]->Int32Value();	//IntegerValue();
  int start = info[1]->Int32Value();	//IntegerValue();
  int length = info[2]->Int32Value();	//IntegerValue();

  sdata = (char *)shmop_read (shmid, start, length);
  if (sdata <= 0) {
    Local<Value> e = NanException(Error, "SHM read error");
    NanReturn(e);
  }
  Local<Object> buf = Nan::NewBuffer(length).ToLocalChecked();
  memcpy(node::Buffer::Data(buf), sdata, length);
  NanReturn(buf);
}

NAN_METHOD(ShmClose) {
//	int  shmop_close (int shmid)
  Nan::HandleScope scope;
  int shmid = info[0]->Int32Value();	//IntegerValue();

  shmop_close (shmid);
  NanReturn(Nan::New<Number>(1));
}

NAN_METHOD(ShmSize) {
//	int  shmop_size (int shmid)
  Nan::HandleScope scope;
  int shmid = info[0]->Int32Value();	//IntegerValue();

  int retval = shmop_size (shmid);
  NanReturn(Nan::New<Number>(retval));
}

NAN_METHOD(ShmWrite) {
//	int  shmop_write (int shmid, char * data, int offset, int data_len)
  Nan::HandleScope scope;
  int shmid = info[0]->Int32Value();	//IntegerValue();
  //Local<String> sdata = info[1]->ToString();
  Local<Value> buffer_v = info[1];
  if (!node::Buffer::HasInstance(buffer_v)) {
    NanThrowException(NanException(TypeError, "Argument should be a buffer"));
  }
  Local<Object> buffer_obj = buffer_v->ToObject();
  char *buffer_data = node::Buffer::Data(buffer_obj);
  int offset = info[2]->Int32Value();	//IntegerValue();
  int data_len = info[3]->Int32Value();	//IntegerValue();

  int retval = shmop_write (shmid, (unsigned char *)buffer_data, offset, data_len);
  NanReturn(Nan::New<Number>(retval));
}

NAN_METHOD(ShmDelete) {
//	int shmop_delete (int shmid)
  Nan::HandleScope scope;
  int shmid = info[0]->Int32Value();	//IntegerValue();

  int retval = shmop_delete (shmid);
  NanReturn(Nan::New<Number>(retval));
}

void Initialize(Local<Object> target) {
  SHMobject::Init();

  target->Set(Nan::New("getErrMsg").ToLocalChecked(),
              Nan::New<FunctionTemplate>(GetErrMsg)->GetFunction());
  target->Set(Nan::New("openSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmOpen)->GetFunction());
  target->Set(Nan::New("readSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmRead)->GetFunction());
  target->Set(Nan::New("closeSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmClose)->GetFunction());
  target->Set(Nan::New("sizeSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmSize)->GetFunction());
  target->Set(Nan::New("writeSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmWrite)->GetFunction());
  target->Set(Nan::New("deleteSHM").ToLocalChecked(),
              Nan::New<FunctionTemplate>(ShmDelete)->GetFunction());
}

NODE_MODULE(shm, Initialize)	//name of module in NODE !!! =  "shm"
