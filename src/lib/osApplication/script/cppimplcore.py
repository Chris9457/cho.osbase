import os.path
from cppbase import CppBase
from apilex import ApiTags as tags


class CppImplCore(CppBase):

    def __init__(self, yamlApi, coreFile, header, apiFile):
        super().__init__(yamlApi, coreFile, apiFile)
        self.header = self.getRelativeTargetedPath(coreFile.name, header)

    def __addHeader(self):
        if self.hasField(self.yamlApi[tags.serviceTag], tags.descriptionTag):
            self.file.write('// \\brief Skeleton implementation - ' + self.yamlApi[tags.serviceTag][tags.descriptionTag] + '\n')
        self.addAutoGeneratedHeader()

    def __addIncludes(self):
        self.file.write('#include "' + self.header + '"\n\n')
        pass

    def __addSkeletonClass(self):
        name = self.yamlApi[tags.serviceTag][tags.nameTag]
        serviceName = name + 'Service'
        skeletonName = serviceName + 'Skeleton'
        baseService = serviceName
        baseClass = 'cho::osbase::application::ServiceImpl<' + baseService + '>'
        if not self.namespace is None:
            baseService = self.namespace + '::' + baseService

        self.file.write('\n    /*\n')
        self.file.write('     * \\class ' + skeletonName + '\n')
        self.file.write('     */\n')
        self.file.write('    ' + skeletonName + '::' + skeletonName +
                        '() : ' + baseClass + '("' +
                        name + 'Service") {\n')
        self.file.write('    }\n\n')

        hasEvent = self.hasField(self.yamlApi, tags.eventsTag)
        if hasEvent:
            for event in self.yamlApi[tags.eventsTag]:
                eventName = event[tags.nameTag]
                self.file.write(
                    '    void ' + skeletonName + '::publish' + eventName + '(')
                if self.hasField(event, tags.typeTag):
                    eventType = self.getCppType(event[tags.typeTag])
                    self.file.write('const ' + eventType + ' &data) const {\n')
                    self.file.write('        publish("' + self.getUri(event, tags.topicTag) + '", data);\n')
                else:
                    eventName = event[tags.nameTag]
                    self.file.write(') const {\n')
                    self.file.write('        publish("' + self.getUri(event, tags.topicTag) + '", ' + eventName + '::type{});\n')

                self.file.write('    }\n\n')

        self.file.write('    void ' + skeletonName + '::doRegister() {\n')
        self.file.write('        ' + baseClass + '::doRegister();\n')
        if self.hasField(self.yamlApi, tags.processTag):
            for method in self.yamlApi[tags.processTag]:
                methodName = method[tags.nameTag]
                self.file.write('        registerCall("' + self.getUri(method, tags.uriTag) + '", this, &' +
                                skeletonName + '::' + methodName + ');\n')
        self.file.write('    }\n\n')

        self.file.write('    void ' + skeletonName + '::doUnregister() {\n')
        if self.hasField(self.yamlApi, tags.processTag):
            for method in reversed(self.yamlApi[tags.processTag]):
                methodName = method[tags.nameTag]
                self.file.write('        unregisterCall("' + self.getUri(method, tags.uriTag) + '");\n')
        self.file.write('        ' + baseClass + '::doUnregister();\n')
        self.file.write('    }\n\n')

    def generate(self):
        self.__addHeader()
        self.__addIncludes()
        self.addNamespace(True)
        self.__addSkeletonClass()
        self.addNamespace(False)
