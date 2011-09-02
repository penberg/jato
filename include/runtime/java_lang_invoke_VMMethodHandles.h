#ifndef JATO__JAVA_LANG_INVOKE_VMMETHOD_HANDLES_H
#define JATO__JAVA_LANG_INVOKE_VMMETHOD_HANDLES_H

#include "vm/jni.h"

jobject java_lang_invoke_VMMethodHandles_findStatic(jobject lookup, jobject refc, jobject name, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findVirtual(jobject lookup, jobject refc, jobject name, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findConstructor(jobject lookup, jobject refc, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findSpecial(jobject lookup, jobject refc, jobject name, jobject method_type, jobject special_caller);
jobject java_lang_invoke_VMMethodHandles_findGetter(jobject lookup, jobject refc, jobject name, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findSetter(jobject lookup, jobject refc, jobject name, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findStaticGetter(jobject lookup, jobject refc, jobject name, jobject method_type);
jobject java_lang_invoke_VMMethodHandles_findStaticSetter(jobject lookup, jobject refc, jobject name, jobject method_type);

#endif /* JATO__JAVA_LANG_INVOKE_VMMETHOD_HANDLES_H */
