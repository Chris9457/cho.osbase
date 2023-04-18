import os.path
from apilex import ApiTags as tags
from cppbase import CppBase


class CppStubCore(CppBase):
    def __init__(self, yamlApi, coreFile, header, apiFile):
        super().__init__(yamlApi, coreFile, apiFile)
        self.header = header

    def __addHeader(self):
        if self.hasField(self.yamlApi[tags.serviceTag], tags.descriptionTag):
            self.file.write('// \\brief ' + self.yamlApi[tags.serviceTag][tags.descriptionTag] + '\n')
        self.addAutoGeneratedHeader()

    def __addIncludes(self):
        headerRelPath = self.getRelativeTargetedPath(self.file.name, self.header)
        self.file.write('#include "' + headerRelPath + '"\n')
        self.file.write('#include "osApplication/ServiceStub.h"\n\n')

    def __getBaseClassName(self, withNamespace):
        serviceName = self.yamlApi[tags.serviceTag][tags.nameTag] + 'Service'
        if withNamespace:
            return 'cho::osbase::application::ServiceStub<' + serviceName + '>'

        return 'ServiceStub<' + serviceName + '>'

    def __addMethod(self, method):
        if self.hasField(method, tags.typeTag):
            methodType = self.getCppType(method[tags.typeTag])
            if methodType is None:
                return
        else:
            methodType = 'void'

        methodName = method[tags.nameTag]
        self.file.write('        ' + methodType + ' ' + methodName + '(')
        argNames = []
        if self.hasField(method, tags.argumentsTag):
            for argument in method[tags.argumentsTag]:
                argName = argument[tags.nameTag]
                if argName in argNames:
                    print('Process: ', methodName, ' - argument \'', argName, '\' already present')
                    return

                argType = self.getCppType(argument[tags.typeTag])
                if argType is None:
                    return

                if len(argNames) != 0:
                    self.file.write(', ')
                self.file.write(argType + ' ' + argName)
                argNames.append(argName)
        self.file.write(') ')
        if self.hasField(method, tags.constTag) and method[tags.constTag]:
            self.file.write('const ')

        self.file.write('override {\n')
        self.file.write('            ')
        if methodType != 'void':
            self.file.write('return ')
        self.file.write(self.__getBaseClassName(False) + '::invoke<' + methodType + '>("' + self.getUri(method, tags.uriTag) + '"')
        for argName in argNames:
            self.file.write(', ' + argName)
        self.file.write(');\n')
        self.file.write('        }\n\n')

    def __addEvents(self, prefix, isReversed):
        if not self.hasField(self.yamlApi, tags.eventsTag):
            return

        writeEvent = lambda evt: self.file.write(
            '            ' + prefix + '<' + evt[tags.nameTag] + '>("' + self.getUri(evt, tags.topicTag) + '");\n')
        if isReversed:
            for event in reversed(self.yamlApi[tags.eventsTag]):
                writeEvent(event)
        else:
            for event in self.yamlApi[tags.eventsTag]:
                writeEvent(event)

    def __addStubClass(self):
        serviceName = self.yamlApi[tags.serviceTag][tags.nameTag] + 'Service'
        stubClassName = serviceName + 'Stub'
        self.file.write('    /*\n')
        self.file.write('     * \\class ' + stubClassName + '\n')
        self.file.write('     */\n')
        self.file.write('    class ' + stubClassName + ' : public ' + self.__getBaseClassName(True) + ' {\n')
        self.file.write('    public:\n')
        self.file.write('        ' +
                        stubClassName +
                        '(cho::osbase::application::TaskLoopPtr pTaskLoop) : ' + self.__getBaseClassName(False)
                        + '(' + '"' + serviceName + '"' + ', pTaskLoop) {}\n\n')

        if tags.processTag in self.yamlApi and not self.yamlApi[tags.processTag] is None:
            for method in self.yamlApi[tags.processTag]:
                self.__addMethod(method)

        self.file.write('    protected:\n')
        self.file.write('        void doRegister() override final {\n')
        self.file.write('            ' + self.__getBaseClassName(False) + '::doRegister();\n')
        self.__addEvents('subscribe', False)
        self.file.write('        }\n\n')

        self.file.write('        void doUnregister() override final {\n')
        self.__addEvents('unsubscribe', True)
        self.file.write('            ' + self.__getBaseClassName(False) + '::doUnregister();\n')
        self.file.write('        }\n')

        self.file.write('    };\n\n')

    def __addMaker(self):
        serviceName = self.yamlApi[tags.serviceTag][tags.nameTag] + 'Service'
        stubClassName = serviceName + 'Stub'
        self.file.write(
            '    ' + serviceName + 'Ptr makeStub(cho::osbase::application::TaskLoopPtr pTaskLoop) {\n')
        self.file.write('        return std::make_shared<' + stubClassName + '>(pTaskLoop);\n')
        self.file.write('    }\n')

    def generate(self):
        self.__addHeader()
        self.__addIncludes()
        self.addNamespace(True)
        self.__addStubClass()
        self.__addMaker()
        self.addNamespace(False)
