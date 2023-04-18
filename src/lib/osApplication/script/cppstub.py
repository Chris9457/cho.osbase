import os
import argparse
import yaml
from cppstubheader import CppStubHeader
from cppstubcore import CppStubCore
from cppexception import CppException


class CppStub:
    def __init__(self, api, outputDir):
        apiFileName = os.path.basename(api)
        apiFileName = os.path.splitext(apiFileName)[0]
        header = os.path.join(outputDir, apiFileName + '.h')
        core = os.path.join(outputDir, apiFileName + '.cpp')
        try:
            with open(api, 'r') as stream:
                yamlApi = yaml.safe_load(stream)
        except FileNotFoundError:
            raise CppException('CppStub.__init__', 'File not found: ' + api)


        try:
            os.makedirs(os.path.dirname(header), exist_ok=True)
            headerFile = open(header, 'w')
        except FileNotFoundError:
            raise CppException('CppStub.__init__', 'Can not create file: ' + header)

        try:
            os.makedirs(os.path.dirname(core), exist_ok=True)
            coreFile = open(core, 'w')
        except FileNotFoundError:
            raise CppException('CppStub.__init__', 'Can not create file: ' + core)

        self.cppHeader = CppStubHeader(yamlApi, headerFile, [api])
        self.cppCore = CppStubCore(yamlApi, coreFile, header, [api])

    def generate(self):
        self.cppHeader.generate()

        self.cppCore.domainObjects = self.cppHeader.domainObjects
        self.cppCore.generate()


def parseCommand():
    parser = argparse.ArgumentParser(description='Generate the api of the service.')
    parser.add_argument('api', help='input api yaml file')
    parser.add_argument('--outputdir', help='output directory of the generated files', required=True)

    return parser.parse_args()


def start():
    args = parseCommand()
    if not args is None:
        try:
            stub = CppStub(args.api, args.outputdir)
            stub.generate()
        except CppException as e:
            print('Exception in: ', e.expression, ' - ', e.message)

start()
