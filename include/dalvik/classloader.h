#ifndef JATO_DALVIK_CLASSLOADER_H
#define JATO_DALVIK_CLASSLOADER_H

struct vm_class;

struct dalvik_classloader {
};

struct dalvik_classloader *dalvik_classloader_new(const char *classpath);

void dalvik_classloader_delete(struct dalvik_classloader *loader);

struct vm_class *dalvik_class_load(struct dalvik_classloader *loader, const char *class_name);

#endif
