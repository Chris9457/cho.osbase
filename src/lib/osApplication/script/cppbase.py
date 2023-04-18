import re
import os.path

import yaml

from apilex import ApiTypes
from apilex import ApiTags as tags
from apilex import ApiAccess
from cppexception import CppException


class CppBase:
    types = {
        ApiTypes.charType: 'char',
        ApiTypes.unsignedCharType: 'unsigned char',
        ApiTypes.shortIntegerType: 'short',
        ApiTypes.unsignedShortIntegerType: 'unsigned short',
        ApiTypes.integerType: 'int',
        ApiTypes.unsignedIntegerType: 'unsigned int',
        ApiTypes.longIntegerType: 'long long',
        ApiTypes.unsignedLongIntegerType: 'unsigned long long',
        ApiTypes.shortFloatingType: 'float',
        ApiTypes.floatingType: 'double',
        ApiTypes.stringType: 'std::string',
        ApiTypes.booleanType: 'bool',
        ApiTypes.uriType: 'cho::osbase::data::Uri'}

    fixCollectionClass = 'std::array'
    dynamicCollectionClass = 'std::vector'

    accessTypes = {
        ApiAccess.asyncAccess: 'cho::osbase::data::AsyncData',
        ApiAccess.asyncPagedAccess: 'cho::osbase::data::AsyncPagedData'
    }

    def __init__(self, yamlApi, file, apiFile):
        self.yamlApi = yamlApi
        self.file = file
        self.apiFile = apiFile
        self.namespace = None
        entry = yamlApi[tags.serviceTag] if tags.serviceTag in yamlApi else yamlApi[tags.moduleTag]
        if tags.namespaceTag in entry:
            self.namespace = entry[tags.namespaceTag]

        if not self.namespace is None:
            self.namespace = self.namespace.replace('.', '::')

        self.__checkMandatoryFields()
        self.domainObjects = self.__loadDomainObjects(yamlApi)
        self.importedDomainObjects = self.__loadImportedDomainObjects(yamlApi)

    def __checkServiceModule(self):
        if self.hasField(self.yamlApi, tags.serviceTag):
            service = self.yamlApi[tags.serviceTag]
            for tag in [tags.nameTag, tags.realmTag]:
                if not self.hasField(service, tag):
                    raise CppException('CppBase.__checkServiceModule',
                                       'Tag "' + '.'.join([tags.serviceTag, tag]) +
                                       '" missing')
        elif self.hasField(self.yamlApi, tags.moduleTag):
            module = self.yamlApi[tags.moduleTag]
            if not self.hasField(module, tags.nameTag):
                raise CppException('CppBase.__checkServiceModule',
                                   'Tag "' + '.'.join([tags.moduleTag, tags.nameTag]) +
                                   '" missing')

    def __checkDomainObjects(self):
        if self.hasField(self.yamlApi, tags.domainObjectsTag):
            domainObjects = self.yamlApi[tags.domainObjectsTag]
            for domainObject in domainObjects:
                if self.hasField(domainObject, tags.structTag):
                    if self.hasField(domainObject, tags.fieldsTag):
                        for field in domainObject[tags.fieldsTag]:
                            for tag in [tags.nameTag, tags.typeTag]:
                                if not self.hasField(field, tag):
                                    raise CppException('CppBase.__checkDomainObjects',
                                                       'Tag "' + tag + '" for the struct "' + domainObject[tags.structTag] +
                                                       '" missing')
                elif self.hasField(domainObject, tags.enumTag):
                    # no mandatory fields
                    pass
                elif self.hasField(domainObject, tags.aliasTag):
                    if not self.hasField(domainObject, tags.typeTag):
                        raise CppException('CppBase.__checkDomainObjects',
                                           'Tag "' + '.'.join(
                                               [tags.domainObjectsTag, tags.aliasTag, tag]) + '" missing')
                else:
                    raise CppException('CppBase.__checkDomainObjects',
                                       'Tag "' + tags.domainObjectsTag + '": unknown domain object')

    def __checkProcess(self):
        uris = []
        if self.hasField(self.yamlApi, tags.processTag):
            for method in self.yamlApi[tags.processTag]:
                if not self.hasField(method, tags.nameTag):
                    raise CppException('CppBase.__checkProcess',
                                       'Tag "' + '.'.join([tags.processTag, tags.nameTag]) + '" missing')
                if not self.hasField(method, tags.uriTag):
                    raise CppException('CppBase.__checkProcess',
                                       'Tag "' + tags.uriTag + '" for the method ' + method[tags.nameTag] + '" missing')
                if method[tags.uriTag] in uris:
                    raise CppException('CppBase.__checkProcess',
                                       'uri "' + method[tags.uriTag] + '" already existing')
                uris.append(method[tags.uriTag])

    def __checkEvents(self):
        topics = []
        if self.hasField(self.yamlApi, tags.eventsTag):
            for event in self.yamlApi[tags.eventsTag]:
                if not self.hasField(event, tags.nameTag):
                    raise CppException('CppBase.__checkMandatoryFields',
                                       'Tag "' + '.'.join([tags.eventsTag, tags.nameTag]) + '" missing')
                if not self.hasField(event, tags.topicTag):
                    raise CppException('CppBase.__checkMandatoryFields',
                                       'Tag "' + tags.topicTag + '" for the event ' + event[tags.nameTag] + '" missing')
                if event[tags.topicTag] in topics:
                    raise CppException('CppBase.__checkEvents',
                                       'topic "' + event[tags.topicTag] + '" already existing')
                topics.append(event[tags.topicTag])

    def __checkMandatoryFields(self):
        if not self.hasField(self.yamlApi, tags.serviceTag) and not self.hasField(self.yamlApi, tags.moduleTag):
            raise CppException('CppBase.__checkMandatoryFields', 'Tag "' + tags.serviceTag + '" or "' + tags.moduleTag +
                               '" missing')

        self.__checkServiceModule()
        self.__checkDomainObjects()
        self.__checkProcess()
        self.__checkEvents()

    def addNamespace(self, begin):
        if self.namespace is None:
            return

        if begin:
            self.file.write('namespace ' + self.namespace + ' {\n')
        else:
            self.file.write('}\n')

    def getLongestCommonNamespace(self, type):
        nsElt = self.namespace.split('::')
        nsTypeElt = type.split('::')

        commonNamespace = ''
        for indexElt in range(len(nsElt)):
            if indexElt > len(nsTypeElt) or nsTypeElt[indexElt] != nsElt[indexElt]:
                return commonNamespace

            if len(commonNamespace) != 0:
                commonNamespace += '::'
            commonNamespace += nsElt[indexElt]

        return commonNamespace

    def getCppType(self, type):
        if type == "":
            raise CppException('CppBase.getCppType', 'empty type: ')

        # Examples allowed:
        #   integer
        #   floating[]
        #   MyDomainObject
        #   mynamespace.MyDomainObject[3]
        #   (async)char[]
        #   (async_paged)unsigned char[]

        match = re.match(r'(\((?P<access>[a-z_]*)\))?'
                         r'(?P<name>[a-zA-Z_][a-zA-Z0-9_. ]*)'
                         r'(?P<occ>(\[(?P<nbocc>[0-9]*)\])?)$', type)
        if not match:
            raise CppException('CppBase.getCppType', 'Type not well formatted: ' + type)

        access = match.group('access')
        typeName = match.group('name')
        occurrence = match.group('occ')
        nbOccurrence = match.group('nbocc')

        hasAccess = not access is None and len(access) != 0
        hasOccurrence = not occurrence is None and len(occurrence) != 0
        hasNbOccurrence = not nbOccurrence is None and len(nbOccurrence) != 0

        if hasAccess and not access in CppBase.accessTypes:
            raise CppException('CppBase.getCppType', 'Unknown access: ' + access)

        if typeName in CppBase.types:
            # Primitive type
            cppType = CppBase.types[typeName]
        else:
            # Custom type
            cppType = typeName.replace('.', '::')

            # check "as is"
            savedCppType = cppType
            isFound = cppType in self.importedDomainObjects or cppType in self.domainObjects
            if not isFound and not self.namespace is None:
                # check with the current namespace
                cppType = self.namespace + '::' + cppType
                isFound = cppType in self.importedDomainObjects or cppType in self.domainObjects

            if not isFound:
                raise CppException('CppBase.getCppType', 'Unknown type: ' + savedCppType)

            # optim: remove the base common namespace
            commonNamespace = self.getLongestCommonNamespace(cppType)
            if len(commonNamespace) != 0:
                cppType = cppType.replace(commonNamespace + '::', '')

        if hasOccurrence:
            if hasNbOccurrence:
                cppType = CppBase.fixCollectionClass + '<' + cppType + ', ' + nbOccurrence + '>'
            else:
                cppType = CppBase.dynamicCollectionClass + '<' + cppType + '>'

        if hasAccess:
            cppType = CppBase.accessTypes[access] + '<' + cppType + '>'

        return cppType

    def getUri(self, element, tag):
        if self.hasField(element, tag):
            return element[tag]

        return self.yamlApi[tags.serviceTag][tags.nameTag] + 'Service.' + element[tags.nameTag]

    def __loadStruct(self, struct):
        structObject = {}
        fieldNames = []

        if self.hasField(struct, tags.fieldsTag):
            for field in struct[tags.fieldsTag]:
                fieldNames.append(field[tags.nameTag])

        structObject[struct[tags.structTag]] = fieldNames
        return fieldNames

    def __loadDomainObjects(self, yamlApi):
        domainObjects = {}
        if self.hasField(yamlApi, tags.domainObjectsTag):
            prefix = ''
            if not self.namespace is None:
                prefix = self.namespace + '::'
            for domainObject in yamlApi[tags.domainObjectsTag]:
                if self.hasField(domainObject, tags.structTag):
                    domainObjects[prefix + domainObject[tags.structTag]] = self.__loadStruct(domainObject)
                elif self.hasField(domainObject, tags.enumTag):
                    domainObjects[prefix + domainObject[tags.enumTag]] = None
                elif self.hasField(domainObject, tags.aliasTag):
                    domainObjects[prefix + domainObject[tags.aliasTag]] = None

        return domainObjects

    def __loadImportedDomainObjects(self, yamlApi):
        if not self.hasField(yamlApi, tags.importsTag):
            return {}

        importedDomainObjects = {}
        saveApiFiles = self.apiFile.copy()
        for importEntry in self.yamlApi[tags.importsTag]:
            if not self.hasField(importEntry, tags.nameTag):
                continue

            importFile = importEntry[tags.nameTag]
            if not os.path.exists(importFile):
                importFile = os.path.abspath(os.path.join(os.path.dirname(self.apiFile[-1]), importFile))
            if not os.path.exists(importFile):
                raise CppException('CppBase.__loadImportedDomainObjects', 'The file "' + importEntry[tags.nameTag] +
                                   '" doesn\'t exist')

            if importFile in self.apiFile:
                continue

            self.apiFile.append(importFile)
            with open(importFile, 'r') as stream:
                yamlImport = yaml.safe_load(stream)
            module = CppBase(yamlImport, self.file, self.apiFile)

            for importedDomainObject in module.domainObjects:
                importedDomainObjects[importedDomainObject] = module.domainObjects[importedDomainObject]

            for importedDomainObject in module.importedDomainObjects:
                importedDomainObjects[importedDomainObject] = module.importedDomainObjects[importedDomainObject]

        self.apiFile = saveApiFiles.copy()
        return importedDomainObjects

    @staticmethod
    def hasField(dictValue, field):
        return type(dictValue) is dict and field in dictValue and not dictValue[field] is None

    @staticmethod
    def getRelativeTargetedPath(fromPath, targetedPath):
        targetedAbsolutePath = os.path.abspath(targetedPath)
        fromDirName = os.path.dirname(os.path.abspath(fromPath))
        targetedRelPath = os.path.relpath(targetedAbsolutePath, fromDirName)
        return targetedRelPath.replace('\\', '/')

    def addAutoGeneratedHeader(self):
        self.file.write('// This file is auto-generated - don\'t modify it manually\n\n')
