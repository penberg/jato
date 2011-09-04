#include "runtime/java_lang_invoke_VMMethodHandles.h"

#include "vm/reflection.h"
#include "vm/errors.h"
#include "vm/method.h"
#include "vm/class.h"
#include "vm/call.h"

#include <stddef.h>
#include <assert.h>

jobject java_lang_invoke_VMMethodHandles_findStatic(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	struct vm_class *vmc;
	unsigned int i;
        char *name_s;

	vmc = vm_object_to_vm_class(refc);
	if (!vmc)
		return rethrow_exception();

	name_s = vm_string_to_cstr(name);
        if (!name_s)
                return throw_oom_error();

	for (i = 0; i < vmc->class->methods_count; i++) {
		struct vm_method *vmm = &vmc->methods[i];
		struct vm_object *mh, *vm_mh;

		if (!vm_method_is_static(vmm))
			continue;

		if (strcmp(name_s, vmm->name))
			continue;

		mh = vm_object_alloc(vm_java_lang_invoke_MethodHandle);
                if (!mh)
                        return rethrow_exception();

                vm_mh = vm_object_alloc(vm_java_lang_invoke_VMMethodHandle);
                if (!vm_mh)
                        return rethrow_exception();

		vm_call_method(vm_java_lang_invoke_MethodHandle_init, mh, vm_mh);

		return mh;
	}

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findVirtual(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	struct vm_class *vmc;
	unsigned int i;
        char *name_s;

	vmc = vm_object_to_vm_class(refc);
	if (!vmc)
		return rethrow_exception();

	name_s = vm_string_to_cstr(name);
        if (!name_s)
                return throw_oom_error();

	for (i = 0; i < vmc->class->methods_count; i++) {
		struct vm_method *vmm = &vmc->methods[i];
		struct vm_object *mh, *vm_mh;

		if (!method_is_virtual(vmm))
			continue;

		if (strcmp(name_s, vmm->name))
			continue;

		mh = vm_object_alloc(vm_java_lang_invoke_MethodHandle);
                if (!mh)
                        return rethrow_exception();

                vm_mh = vm_object_alloc(vm_java_lang_invoke_VMMethodHandle);
                if (!vm_mh)
                        return rethrow_exception();

		vm_call_method(vm_java_lang_invoke_MethodHandle_init, mh, vm_mh);

		return mh;
	}

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findConstructor(jobject lookup, jobject refc, jobject method_type)
{
	assert(0);

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findSpecial(jobject lookup, jobject refc, jobject name, jobject method_type, jobject special_caller)
{
	assert(0);

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findGetter(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	assert(0);

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findSetter(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	assert(0);

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findStaticGetter(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	assert(0);

	return NULL;
}

jobject java_lang_invoke_VMMethodHandles_findStaticSetter(jobject lookup, jobject refc, jobject name, jobject method_type)
{
	assert(0);

	return NULL;
}
