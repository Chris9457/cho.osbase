// \brief Declaration of the class StructuredExceptionHandler

#pragma once
#include "RuntimeException.h"
#include "osCore/DesignPattern/Singleton.h"
#include <filesystem>
#include <functional>

namespace cho::osbase::core {

    constexpr char DUMPFORMAT[]       = "%Y%m%d_%H%M%S.dmp";
    constexpr char DUMPBASEFILENAME[] = "dump_";

    /**
     * \brief Exception re-thrown when a stuctured exception is caught
     * \ingroup PACKAGE_EXCEPTION
     */
    class StructuredException : public RuntimeException {
        using RuntimeException::RuntimeException;
    };

    /**
     * \brief This class provides methods to handle Structured Exception
     * \ingroup PACKAGE_EXCEPTION
     */
    class StructuredExceptionHandler : public Singleton<StructuredExceptionHandler> {
        friend Singleton<StructuredExceptionHandler>;

    public:
        const std::filesystem::path &getMinidumpFolder() const;      //!< return the current minidump folder
        void setMinidumpFolder(const std::filesystem::path &folder); //!< set the minidump folder

        const std::string getDumpBaseFileName() const;             //!< return the base file name of the minidump file
        void setDumpBaseFileName(const std::string &baseFileName); //!< set the base file name of the minidump file

        /**
         * \brief check in the function invocation doesn't crash (or generate a SEH)
         * \param f     function to chack
         * \param args  arguments of the function
         * \return the result of f
         * \throw StructuredException if SEH
         */
        template <typename TRet, typename TFunc, typename... TArgs>
        TRet checkStructuredException(const TFunc &f, TArgs &&...args) const;

    private:
        StructuredExceptionHandler();
        ~StructuredExceptionHandler() override = default;

        std::filesystem::path getDumpFileName() const;
        void checkStructuredException(const std::function<void()> &function) const;

        std::filesystem::path m_miniDumpFolder;
        std::string m_dumpBaseFileName = DUMPBASEFILENAME;
    };

#define TheStructuredExceptionHandler StructuredExceptionHandler::getInstance()
} // namespace cho::osbase::core

#include "StructuredExceptionHandler.inl"
