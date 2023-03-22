#include <linux/module.h>
#include <linux/kernel.h>   /*para usar KERN_INFO*/
#include <linux/init.h>     /* Header para los macros module_init y module_exit */
#include <linux/proc_fs.h>  /* Header necesario porque se usara proc_fs */
#include <asm/uaccess.h>    /* for copy_from_user */
#include <linux/seq_file.h> /* Header para usar la lib seq_file y manejar el archivo en /proc*/

/* libreria de memoria ram*/
#include <linux/hugetlb.h>

#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Práctica 2, Laboratorio Sistemas Operativos 1 - Sección A ");
MODULE_AUTHOR("BYRON RUBÉN HERNÁNDEZ DE LEÓN");

struct task_struct * task;
struct task_struct * task_child;
struct list_head * list;

/* Función que muestra el contenido del comando CAT */
static int write_file(struct seq_file *archivo, void *v)
{   

    seq_printf(archivo, "[\n");
    bool pid_first = true;

    for_each_process(task){

        if(pid_first){
            seq_printf(archivo, "\t{\n");
            pid_first = false;
        }else{
            seq_printf(archivo, ",\n\t{\n");
        }
        seq_printf(archivo, "\t\t\"pid\":%d,\n",        task->pid);
        seq_printf(archivo, "\t\t\"name\":\"%s\",\n",   task->comm);
        seq_printf(archivo, "\t\t\"state\":%d,\n",      task->__state);
        seq_printf(archivo, "\t\t\"user\":%d,\n",       task->cred->user->uid.val);
        seq_printf(archivo, "\t\t\"ram\":%d,\n",        task->mm);
        seq_printf(archivo, "\t\t\"parent\":0,\n");
        seq_printf(archivo, "\t\t\"childs\":[\n");
        
        bool first_child = true;
        list_for_each(list, &(task->children)){
            task_child = list_entry( list, struct task_struct, sibling );

            if(!first_child){
                seq_printf(archivo, ",\n");
            }

            first_child = false;
            seq_printf(archivo, "\t\t\t\t{\n");

            seq_printf(archivo, "\t\t\t\t\t\"pid\":%d,\n",        task_child->pid);
            seq_printf(archivo, "\t\t\t\t\t\"name\":\"%s\",\n",   task_child->comm);
            seq_printf(archivo, "\t\t\t\t\t\"state\":%d,\n",      task_child->__state);
            seq_printf(archivo, "\t\t\t\t\t\"user\":%d,\n",       task_child->cred->user->uid.val);
            seq_printf(archivo, "\t\t\t\t\t\"ram\":%d,\n",        task_child->mm);
            seq_printf(archivo, "\t\t\t\t\t\"parent\":%d\n",      task->pid);

            seq_printf(archivo, "\t\t\t\t}");

        }
        seq_printf(archivo, "\n\t\t]\n\t}");

    }
    seq_printf(archivo, "\n]\n");
    return 0;
}

/* Está función se encarga de ejecutarse cada ver que se lea el archivo con el comando CAT */
static int al_abrir(struct inode *inode, struct file *file)
{
    return single_open(file, write_file, NULL);
}

/* Si el kernel es 5.6 o mayor se usa la estructura proc_ops */
static struct proc_ops operaciones =
{
    .proc_open = al_abrir,
    .proc_read = seq_read
};

/* Está función ejecuta al insertar el módulo en el kernel con el comando insmod */
static int _insert(void)
{
    proc_create("cpu_201806840", 0, NULL, &operaciones);
    printk(KERN_INFO "BYRON RUBÉN HERNÁNDEZ DE LEÓN\n");
    return 0;
}

/* Está Función se ejecuta al remover el modulo del kernel con el comadno rmmod */
static void _remove(void)
{
    remove_proc_entry("cpu_201806840", NULL);
    printk(KERN_INFO "primer semestre 2023\n");
}

module_init(_insert);
module_exit(_remove);