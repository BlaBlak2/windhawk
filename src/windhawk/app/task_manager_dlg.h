#pragma once

#include "resource.h"

class CTaskManagerDlg : public CDialogImpl<CTaskManagerDlg>,
                        public CDialogResize<CTaskManagerDlg> {
   public:
    enum { IDD = IDD_TASK_MANAGER };

    BEGIN_DLGRESIZE_MAP(CTaskManagerDlg)
        DLGRESIZE_CONTROL(IDC_TASK_LIST, DLSZ_SIZE_X | DLSZ_SIZE_Y)
        DLGRESIZE_CONTROL(IDOK, DLSZ_MOVE_X | DLSZ_MOVE_Y)
    END_DLGRESIZE_MAP()

    enum class DataSource {
        kModStatus,
        kModTask,
    };

    using DlgCallback = std::function<void(HWND)>;

    // Wait before showing the autonomous dialog in case the data is short
    // lived.
    static constexpr int kAutonomousModeShowDelayDefault = 2000;
    static constexpr int kAutonomousModeShowDelayMin = 400;

    struct DialogOptions {
        DataSource dataSource = DataSource::kModStatus;
        bool autonomousMode = false;
        int autonomousModeShowDelay = kAutonomousModeShowDelayDefault;
        DWORD sessionManagerProcessId{};
        ULONGLONG sessionManagerProcessCreationTime{};
        DlgCallback runButtonCallback;
        DlgCallback finalMessageCallback;
    };

    static bool IsDataSourceEmpty(DataSource dataSource);

    CTaskManagerDlg(DialogOptions dialogOptions);

    void LoadLanguageStrings();
    void DataChanged();

   private:
    enum class Timer {
        kUpdateProcessesStatus = 1,
        kRefreshList,
        kShowDlg,
    };

    BEGIN_MSG_MAP_EX(CTaskManagerDlg)
        CHAIN_MSG_MAP(CDialogResize<CTaskManagerDlg>)
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_DESTROY(OnDestroy)
        MSG_WM_TIMER(OnTimer)
        MSG_WM_DPICHANGED(OnDpiChanged)
        COMMAND_ID_HANDLER_EX(IDOK, OnOK)
        COMMAND_ID_HANDLER_EX(IDCANCEL, OnCancel)
        NOTIFY_HANDLER_EX(IDC_TASK_LIST, NM_RCLICK, OnListRightClick)
    END_MSG_MAP()

    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnDestroy();
    void OnTimer(UINT_PTR nIDEvent);
    void OnDpiChanged(UINT nDpiX, UINT nDpiY, PRECT pRect);
    void OnOK(UINT uNotifyCode, int nID, CWindow wndCtl);
    void OnCancel(UINT uNotifyCode, int nID, CWindow wndCtl);
    LRESULT OnListRightClick(LPNMHDR pnmh);

    void OnFinalMessage(HWND hWnd) override;
    UINT_PTR SetTimer(Timer nIDEvent,
                      UINT nElapse,
                      TIMERPROC lpfnTimer = nullptr);
    BOOL KillTimer(Timer nIDEvent);
    void ReloadMainIcon();
    void PlaceWindowAtTrayArea();
    void InitTaskList();
    void LoadTaskList();
    bool LoadTaskItemFromMetadataFile(const std::filesystem::path& filePath,
                                      int itemIndex);
    void AddItemToList(int itemIndex,
                       PCWSTR filePath,
                       PCWSTR mod,
                       PCWSTR processName,
                       DWORD processId,
                       PCWSTR status,
                       FILETIME creationTime);
    void RefreshTaskList();
    void UpdateTaskListProcessesStatus();
    void UpdateDialogAfterListUpdate();

    const DialogOptions m_dialogOptions;
    CSortListViewCtrl m_taskListSort;
    bool m_refreshListOnDataChangePending = false;
    bool m_showDlgPending = false;
};
