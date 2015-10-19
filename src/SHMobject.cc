#if !defined BUILDING_NODE_EXTENSION
	#define BUILDING_NODE_EXTENSION
#endif
#include <node.h>
#include <nan.h>
#include "SHMobject.h"

using namespace v8;

SHMobject::SHMobject() {};
SHMobject::~SHMobject() {};

Nan::Persistent<Function> SHMobject::constructor;

void SHMobject::Init() {
  // Prepare constructor template
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("SHMobject").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(tpl->GetFunction());
}

NAN_METHOD(SHMobject::New) {
  Nan::HandleScope scope;

  SHMobject* obj = new SHMobject();
  obj->val_ = info[0]->IsUndefined() ? 0 : info[0]->NumberValue();
  obj->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
  return;
}

NAN_METHOD(SHMobject::NewInstance) {
  Nan::HandleScope scope;

  Local<Object> instance = Nan::New(constructor)->NewInstance();
  info.GetReturnValue().Set(instance);
  return;
}
