
/* DO NOT EDIT.
 *
 * This file is autogenerated by
 * "gentypes.py libxl.idl __libxl_types.h __libxl_types.c"
 */

#include "libxl_osdeps.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libxl.h"

#define LIBXL_DTOR_POISON 0xa5

void libxl_cpupoolinfo_destroy(libxl_cpupoolinfo *p)
{
    libxl_cpumap_destroy(&p->cpumap);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_version_info_destroy(libxl_version_info *p)
{
    free(p->xen_version_extra);
    free(p->compiler);
    free(p->compile_by);
    free(p->compile_domain);
    free(p->compile_date);
    free(p->capabilities);
    free(p->changeset);
    free(p->commandline);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_domain_create_info_destroy(libxl_domain_create_info *p)
{
    free(p->name);
    libxl_key_value_list_destroy(&p->xsdata);
    libxl_key_value_list_destroy(&p->platformdata);
    free(p->poolname);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_domain_build_info_destroy(libxl_domain_build_info *p)
{
    libxl_file_reference_destroy(&p->kernel);
    libxl_cpuid_destroy(&p->cpuid);
    if (p->hvm) {
        free(p->u.hvm.timeoffset);
    }
    if (!p->hvm) {
        free(p->u.pv.bootloader);
        free(p->u.pv.bootloader_args);
        free(p->u.pv.cmdline);
        libxl_file_reference_destroy(&p->u.pv.ramdisk);
    }
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_model_info_destroy(libxl_device_model_info *p)
{
    free(p->dom_name);
    free(p->device_model);
    free(p->saved_state);
    free(p->vnclisten);
    free(p->vncpasswd);
    free(p->keymap);
    free(p->serial);
    free(p->boot);
    free(p->usbdevice);
    free(p->soundhw);
    libxl_string_list_destroy(&p->extra);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_vfb_destroy(libxl_device_vfb *p)
{
    free(p->vnclisten);
    free(p->vncpasswd);
    free(p->keymap);
    free(p->display);
    free(p->xauthority);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_vkb_destroy(libxl_device_vkb *p)
{
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_console_destroy(libxl_device_console *p)
{
    free(p->output);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_disk_destroy(libxl_device_disk *p)
{
    free(p->pdev_path);
    free(p->vdev);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_nic_destroy(libxl_device_nic *p)
{
    free(p->model);
    free(p->ip);
    free(p->bridge);
    free(p->ifname);
    free(p->script);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_net2_destroy(libxl_device_net2 *p)
{
    free(p->bridge);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_device_pci_destroy(libxl_device_pci *p)
{
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_diskinfo_destroy(libxl_diskinfo *p)
{
    free(p->backend);
    free(p->frontend);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_nicinfo_destroy(libxl_nicinfo *p)
{
    free(p->backend);
    free(p->frontend);
    free(p->script);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_vcpuinfo_destroy(libxl_vcpuinfo *p)
{
    libxl_cpumap_destroy(&p->cpumap);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_topologyinfo_destroy(libxl_topologyinfo *p)
{
    libxl_cpuarray_destroy(&p->coremap);
    libxl_cpuarray_destroy(&p->socketmap);
    libxl_cpuarray_destroy(&p->nodemap);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}

void libxl_net2info_destroy(libxl_net2info *p)
{
    free(p->backend);
    free(p->frontend);
    memset(p, LIBXL_DTOR_POISON, sizeof(*p));
}
