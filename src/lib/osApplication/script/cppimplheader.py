import os.path
from cppbase import CppBase
from apilex import ApiTags as tags


class CppImplHeader(CppBase):

    def __init__(self, yamlApi, headerFile, apiFile):
        super().__init__(yamlApi, headerFile, apiFile)
        self.stubHeader = os.path.splitext(os.path.basename(apiFile[0]))[0] + '.h'

    def __addHeader(self):
        if self.hasField(self.yamlApi[tags.serviceTag], tags.descriptionTag):
            self.file.write('// \\brief Skeleton implementation - ' + self.yamlApi[tags.serviceTag][tags.descriptionTag] + '\n')
        self.addAutoGeneratedHeader()
        self.file.write('#pragma once\n')

    def __addIncludes(self):
        self.file.write('#include "' + self.stubHeader + '"\n')
        self.file.write('#include "osApplication/ServiceImpl.h"\n\n')

    def __addSkeletonClass(self):
        name = self.yamlApi[tags.serviceTag][tags.nameTag]
        serviceName = name + 'Service'
        skeletonName = serviceName + 'Skeleton'

        self.file.write('\n    /**\n')
        self.file.write('     * \\brief Skeleton declaration\n')
        self.file.write('     */\n')
        self.file.write('    class ' + skeletonName + ' : public cho::osbase::application::ServiceImpl<' +
                        serviceName + '> {\n')
        self.file.write('    protected:\n')
        self.file.write('        ' + skeletonName + '();\n\n')

        hasEvent = self.hasField(self.yamlApi, tags.eventsTag)
        if hasEvent:
            for event in self.yamlApi[tags.eventsTag]:
                eventName = event[tags.nameTag]
                self.file.write('        void publish' + eventName + '(')
                if self.hasField(event, tags.typeTag):
                    eventType = self.getCppType(event[tags.typeTag])
                    self.file.write('const ' + eventType + ' &data')
                self.file.write(') const;\n')

        if hasEvent:
            self.file.write('\n')

        self.file.write('        void doRegister() override final;\n')
        self.file.write('        void doUnregister () override final;\n')
        self.file.write('    };\n')

    def generate(self):
        self.__addHeader()
        self.__addIncludes()
        self.addNamespace(True)
        self.__addSkeletonClass()
        self.addNamespace(False)
