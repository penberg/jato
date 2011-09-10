#include "runtime/java_lang_invoke_VMMethodHandle.h"

#include "vm/reflection.h"
#include "vm/preload.h"
#include "vm/method.h"
#include "vm/object.h"
#include "vm/call.h"

#include <stddef.h>
#include <assert.h>

jobject java_lang_invoke_VMMethodHandle_invoke(jobject this, jobject args)
{
	struct vm_method *vmm;
	struct vm_object *ptr;

	assert(this != NULL);

	ptr = field_get_object(this, vm_java_lang_invoke_VMMethodHandle_ptr);

	assert(ptr != NULL);

#ifdef CONFIG_32_BIT
	vmm = (void *) field_get_int(ptr, vm_gnu_classpath_PointerNN_data);
#else
	vmm = (void *) field_set_long(ptr, vm_gnu_classpath_PointerNN_data);
#endif
	assert(vmm != NULL);

	if (method_is_virtual(vmm)) {
		struct vm_object *o = NULL;

		o = array_get_field_ptr(args, 0);

		assert(o != NULL);

		return call_virtual_method(vmm, o, args, 1);
	}

	return call_static_method(vmm, args);
}
