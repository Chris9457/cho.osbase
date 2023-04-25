// \brief Declaration of the class CrashHandler

#include "osCore/Exception/StructuredExceptionHandler.h"
#include <Windows.h>
#include <DbgHelp.h>

namespace NS_OSBASE::core {

    namespace {
        bool dumpFile(const wchar_t *dumpFileName, void *pExceptionInfo) {
            // Remark: this method is used within the snetence "__except" and so doesn't support the unwinding objects (destruction)
            // So only C-style implementation compile in this method

            using MiniDumpWriteDump_t = BOOL (*)(HANDLE,
                DWORD,
                HANDLE,
                MINIDUMP_TYPE,
                PMINIDUMP_EXCEPTION_INFORMATION,
                PMINIDUMP_USER_STREAM_INFORMATION,
                PMINIDUMP_CALLBACK_INFORMATION);

            auto const hDbgHelp = ::LoadLibrary(L"Dbghelp.dll");
            if (hDbgHelp == nullptr) {
                return false;
            }

            auto const hFile =
                ::CreateFile(dumpFileName, GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

            if (hFile == INVALID_HANDLE_VALUE) {
                ::FreeLibrary(hDbgHelp);
                return false;
            }

            auto const miniDumpWriteDump = reinterpret_cast<MiniDumpWriteDump_t>(GetProcAddress(hDbgHelp, "MiniDumpWriteDump"));
            auto const hProc             = ::GetCurrentProcess();
            auto const pid               = ::GetCurrentProcessId();
            MINIDUMP_EXCEPTION_INFORMATION exInfo;
            exInfo.ThreadId          = ::GetCurrentThreadId();
            exInfo.ExceptionPointers = static_cast<PEXCEPTION_POINTERS>(pExceptionInfo);
            auto const bResult       = miniDumpWriteDump(hProc, pid, hFile, MiniDumpNormal, &exInfo, nullptr, nullptr);
            ::CloseHandle(hFile);
            ::FreeLibrary(hDbgHelp);

            return bResult;
        }

        int filterStructuredException(
            const wchar_t *dumpFileName, unsigned int code, PEXCEPTION_POINTERS pExp, char *exceptionLabel, const size_t labelSize) {
            if (!dumpFile(dumpFileName, pExp)) {
                strcpy_s(exceptionLabel, labelSize, "Unable to write the dumpfile - ");
            }

            sprintf_s(exceptionLabel, labelSize, "(0x%08X) ", code);

            switch (code) {
            case EXCEPTION_ACCESS_VIOLATION:
                strcat_s(
                    exceptionLabel, labelSize, "The thread tries to read or write to a virtual address for which it does not have access.");
                break;
            case EXCEPTION_DATATYPE_MISALIGNMENT:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread attempts to read or write data that is misaligned to the hardware that does not provide alignment. For "
                    "example, 16-bit values must be aligned with 2-byte limits, 32-bit values with 4-byte limits, and so on.");
                break;
            case EXCEPTION_BREAKPOINT:
                strcat_s(exceptionLabel, labelSize, "A breakpoint was encountered.");
                break;
            case EXCEPTION_SINGLE_STEP:
                strcat_s(
                    exceptionLabel, labelSize, "A trace trap or other single instruction mechanism signals that a statement is executed.");
                break;
            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread attempts to access an out-of-bounds array item, and the underlying hardware supports limit checking.");
                break;
            case EXCEPTION_FLT_DENORMAL_OPERAND:
                strcat_s(exceptionLabel,
                    labelSize,
                    "One of the operands of a floating-point operation is denormal. A denormal value is a value that is too small to "
                    "represent as a standard floating-point value.");
                break;
            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread attempts to divide a floating-point value by a floating-point divider of 0 (zero).");
                break;
            case EXCEPTION_FLT_INEXACT_RESULT:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The result of a floating-point operation cannot be represented exactly as a decimal fraction.");
                break;
            case EXCEPTION_FLT_INVALID_OPERATION:
                strcat_s(exceptionLabel, labelSize, "Floating-point exception that is not included in this list.");
                break;
            case EXCEPTION_FLT_OVERFLOW:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The exponent of a floating-point operation is greater than the magnitude allowed by the corresponding type.");
                break;
            case EXCEPTION_FLT_STACK_CHECK:
                strcat_s(exceptionLabel, labelSize, "The stack has outgrown or underflow, due to a floating-point operation.");
                break;
            case EXCEPTION_FLT_UNDERFLOW:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The exponent of a floating-point operation is less than the magnitude allowed by the corresponding type.");
                break;
            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                strcat_s(exceptionLabel, labelSize, "The thread attempts to divide an integer value by an integer divisor of 0 (zero).");
                break;
            case EXCEPTION_INT_OVERFLOW:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The result of an entire operation creates too much value for the destination ledger to keep. In some cases, this "
                    "results in the most significant bit of the result being executed. Some operations do not define the transport "
                    "indicator.");
                break;
            case EXCEPTION_PRIV_INSTRUCTION:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread attempts to execute a statement with an operation that is not allowed in current computer mode.");
                break;
            case EXCEPTION_IN_PAGE_ERROR:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread tries to access a page that is not present and the system cannot load the page. For example, this "
                    "exception may occur if a network connection is lost while running a program on a network.");
                break;
            case EXCEPTION_ILLEGAL_INSTRUCTION:
                strcat_s(exceptionLabel, labelSize, "The thread attempts to execute an invalid statement.");
                break;
            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                strcat_s(exceptionLabel, labelSize, "The thread attempts to continue execution after a non-continuing exception occurs.");
                break;
            case EXCEPTION_STACK_OVERFLOW:
                strcat_s(exceptionLabel, labelSize, "The thread uses its stack.");
                break;
            case EXCEPTION_INVALID_DISPOSITION:
                strcat_s(exceptionLabel,
                    labelSize,
                    "An exception handler returns an invalid provision to the exception breaker. Programmers using a high-level "
                    "language such as C should never encounter this exception.");
                break;
            case EXCEPTION_GUARD_PAGE:
                strcat_s(exceptionLabel, labelSize, "The thread accessed the allocated memory with the PAGE_GUARD modifier.");
                break;
            case EXCEPTION_INVALID_HANDLE:
                strcat_s(exceptionLabel,
                    labelSize,
                    "The thread used a handle for a kernel object that was invalid (probably because it had been closed.)");
                break;
            default:
                strcat_s(exceptionLabel, labelSize, "Unknown structured exception.");
                break;
            }

            return EXCEPTION_EXECUTE_HANDLER;
        }

        void checkSE(const std::function<void()> &function, const wchar_t *dumpFileName) {
            char exceptionLabel[1024];
            __try {
                function();
            } __except (filterStructuredException(
                dumpFileName, GetExceptionCode(), GetExceptionInformation(), exceptionLabel, sizeof(exceptionLabel))) {
                throw StructuredException(exceptionLabel);
            }
        }
    } // namespace

    /*
     * \class StructuredExceptionHandler
     */
    StructuredExceptionHandler::StructuredExceptionHandler() : m_miniDumpFolder(std::filesystem::current_path()) {
    }

    const std::filesystem::path &StructuredExceptionHandler::getMinidumpFolder() const {
        return m_miniDumpFolder;
    }

    void StructuredExceptionHandler::setMinidumpFolder(const std::filesystem::path &folder) {
        m_miniDumpFolder = folder;
        create_directories(folder);
    }

    const std::string StructuredExceptionHandler::getDumpBaseFileName() const {
        return m_dumpBaseFileName;
    }

    void StructuredExceptionHandler::setDumpBaseFileName(const std::string &baseFileName) {
        m_dumpBaseFileName = baseFileName;
    }

    void StructuredExceptionHandler::checkStructuredException(const std::function<void()> &function) const {
        checkSE(function, getDumpFileName().wstring().c_str());
    }

    std::filesystem::path StructuredExceptionHandler::getDumpFileName() const {
        using clock = std::chrono::system_clock;

        std::stringstream os;
        auto const tt = clock::to_time_t(clock::now());
        auto const tm = *std::localtime(&tt);

        os << std::put_time(&tm, (m_dumpBaseFileName + DUMPFORMAT).c_str());
        return m_miniDumpFolder / os.str();
    }
} // namespace NS_OSBASE::core
