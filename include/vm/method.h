#ifndef __VM_METHOD_H
#define __VM_METHOD_H

#include "vm/vm.h"
#include "vm/types.h"

#include <stdbool.h>
#include <string.h>

#include "cafebabe/code_attribute.h"
#include "cafebabe/class.h"
#include "cafebabe/line_number_table_attribute.h"
#include "cafebabe/method_info.h"

#include "jit/compilation-unit.h"
#include "jit/compiler.h"

#include "lib/buffer.h"

struct vm_class;

struct vm_method {
	struct vm_class *class;
	unsigned int method_index;
	unsigned int virtual_index;
	unsigned int itable_index;
	const struct cafebabe_method_info *method;

	char *name;
	char *type;
	int args_count;
#ifdef	CONFIG_REGPARM
	int reg_args_count;
#endif

	struct cafebabe_code_attribute code_attribute;
	struct cafebabe_line_number_table_attribute line_number_table_attribute;

	struct compilation_unit *compilation_unit;
	struct jit_trampoline *trampoline;

	bool is_vm_native;
};

int vm_method_init(struct vm_method *vmm,
	struct vm_class *vmc, unsigned int method_index);

static inline bool vm_method_is_static(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_STATIC;
}

static inline bool vm_method_is_native(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_NATIVE;
}

static inline bool vm_method_is_abstract(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_ABSTRACT;
}

static inline bool method_is_synchronized(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_SYNCHRONIZED;
}

static inline bool method_is_private(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_PRIVATE;
}

static inline bool vm_method_is_constructor(struct vm_method *vmm)
{
	return strcmp(vmm->name, "<init>") == 0;
}

static inline bool method_is_virtual(struct vm_method *vmm)
{
	if (vm_method_is_constructor(vmm))
		return false;

	return (vmm->method->access_flags & (CAFEBABE_METHOD_ACC_STATIC
		| CAFEBABE_METHOD_ACC_PRIVATE)) == 0;
}

static inline bool vm_method_is_jni(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_NATIVE
		&& !vmm->is_vm_native;
}

static inline bool vm_method_is_vm_native(struct vm_method *vmm)
{
	return vmm->method->access_flags & CAFEBABE_METHOD_ACC_NATIVE
		&& vmm->is_vm_native;
}

static inline enum vm_type method_return_type(struct vm_method *method)
{
	char *return_type = method->type + (strlen(method->type) - 1);
	return str_to_type(return_type);
}

int vm_method_prepare_jit(struct vm_method *vmm);

static inline void *vm_method_native_ptr(struct vm_method *vmm)
{
	return vmm->compilation_unit->native_ptr;
}

static inline void *vm_method_trampoline_ptr(struct vm_method *vmm)
{
	return buffer_ptr(vmm->trampoline->objcode);
}

static inline void *vm_method_call_ptr(struct vm_method *vmm)
{
	/*
	 * We don't need to lock on compilation unit here because
	 * ->is_compiled can only change its value from false to true
	 * and before it is set to true the value of ->native_ptr is set
	 * and is constant after that.
	 */
	if (vmm->compilation_unit->is_compiled)
		return vm_method_native_ptr(vmm);

	return vm_method_trampoline_ptr(vmm);
}

#endif
