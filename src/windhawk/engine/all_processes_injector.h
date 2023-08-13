#pragma once

class AllProcessesInjector {
   public:
    AllProcessesInjector(bool skipCriticalProcesses);

    int InjectIntoNewProcesses() noexcept;

   private:
    bool ShouldSkipNewProcess(HANDLE hProcess,
                              DWORD dwProcessId,
                              bool* threadAttachExempt);
    bool ShouldSkipCriticalProcess(DWORD dwProcessId);
    void InjectIntoNewProcess(HANDLE hProcess,
                              DWORD dwProcessId,
                              bool threadAttachExempt);

    using NtGetNextProcess_t = NTSTATUS(NTAPI*)(_In_opt_ HANDLE ProcessHandle,
                                                _In_ ACCESS_MASK DesiredAccess,
                                                _In_ ULONG HandleAttributes,
                                                _In_ ULONG Flags,
                                                _Out_ PHANDLE NewProcessHandle);

    using NtGetNextThread_t = NTSTATUS(NTAPI*)(_In_ HANDLE ProcessHandle,
                                               _In_opt_ HANDLE ThreadHandle,
                                               _In_ ACCESS_MASK DesiredAccess,
                                               _In_ ULONG HandleAttributes,
                                               _In_ ULONG Flags,
                                               _Out_ PHANDLE NewThreadHandle);

    bool m_skipCriticalProcesses = false;
    NtGetNextProcess_t m_NtGetNextProcess;
    NtGetNextThread_t m_NtGetNextThread;
    DWORD64 m_pRtlUserThreadStart;
    wil::unique_private_namespace_destroy m_appPrivateNamespace;
    std::wstring m_includePattern;
    std::wstring m_excludePattern;
    std::wstring m_threadAttachExemptPattern;
    std::wstring m_criticalProcessesPattern;
    wil::unique_process_handle m_lastEnumeratedProcess;
};
