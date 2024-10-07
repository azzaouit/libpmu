#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/smp.h>

static void pce_set(void *info) {
    asm volatile (
            "push %rax;"
            "mov %cr4, %rax;"
            "or $0x100, %rax;"
            "mov %rax, %cr4;"
            "pop %rax;");
    pr_info("[%d] PCE bit set\n", smp_processor_id());
}

static void pce_clear(void *info) {
    asm volatile (
        "push   %rax;"
        "push   %rbx;"
        "mov    %cr4, %rax;"
        "mov    $0x100, %rbx;"
        "not    %rbx;"
        "and    %rbx, %rax;"
        "mov    %rax, %cr4;"
        "pop    %rbx;"
        "pop    %rax;");
    pr_info("[%d] PCE bit clear\n", smp_processor_id());
}

static int __init  mod_init(void) {
    on_each_cpu(pce_set, NULL, 0);
    return 0;
}

static void __exit mod_exit(void) {
    on_each_cpu(pce_clear, NULL, 0);
}

module_init(mod_init);
module_exit(mod_exit);
MODULE_LICENSE("GPL");
