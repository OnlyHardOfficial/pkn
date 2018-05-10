#pragma once

#include <intrin.h>
#include <winternl.h>
#include "disable_windows_min_max_definetion.h"

#include <vector>
#include <stdexcept>
#include <string>
#include <Psapi.h>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <map>

#include <ppl.h>

#include "../base/types.h"
#include "../driver_control/Driver.h"
#include "IProcess.h"
#include "IAddressableProcess.h"
#include "../base/noncopyable.h"

namespace pkn
{
    class Driver;

    class KernelProcessBase : public noncopyable
    {
    public:
        KernelProcessBase(pid_t pid);
        virtual ~KernelProcessBase() = default;
    public:
        inline euint64_t pid() const { return _pid; };
        inline Driver &driver() const { return _driver; }
    private:
        pid_t _pid = 0;
        Driver &_driver;
    };

    class KernelBasicProcess : virtual public KernelProcessBase, virtual public IBasicProcess
    {
    public:
        KernelBasicProcess(pid_t pid);
        virtual ~KernelBasicProcess() override = default;
    public:
        virtual erptr_t base() const override;
        virtual bool alive() const override;
    protected:
        erptr_t _base;
    };

    class KernelReadableProcess : virtual public KernelProcessBase, virtual public IReadableProcess
    {
    public:
        KernelReadableProcess(pid_t pid) : KernelProcessBase(pid) {}
        virtual ~KernelReadableProcess() override = default;
    public:
        virtual void read_unsafe(erptr_t address, size_t size, void *buffer) const override;
    };

    class KernelWritableProcess : virtual public KernelProcessBase, virtual public IWritableProcess
    {
    public:
        KernelWritableProcess(pid_t pid) : KernelProcessBase(pid) {}
        virtual ~KernelWritableProcess() override = default;
    public:
        virtual void write_unsafe(erptr_t address, size_t size, const void *buffer) const override;
    };

    class KernelProcessRegions : virtual public KernelProcessBase, virtual public IProcessRegions
    {
    public:
        KernelProcessRegions(pid_t pid) : KernelProcessBase(pid) {}
        virtual ~KernelProcessRegions() override = default;
    public:
        virtual MemoryRegions get_all_memory_regions() override;
        virtual estr_t get_mapped_file(erptr_t remote_address) const override;
    };

    class KernelProcess
        : public virtual KernelBasicProcess,
        public virtual KernelReadableProcess,
        public virtual KernelWritableProcess,
        public virtual KernelProcessRegions,
        public virtual ProcessAddressTypeJudger
    {
    public:
        KernelProcess(pid_t pid) :
            KernelProcessBase(pid),
            KernelBasicProcess(pid),
            KernelReadableProcess(pid),
            KernelWritableProcess(pid),
            KernelProcessRegions(pid),
            ProcessAddressTypeJudger(*this) {}
        virtual ~KernelProcess() override = default;
    };
}