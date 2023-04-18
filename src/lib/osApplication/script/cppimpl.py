import os
import argparse
import yaml
from cppimplheader import CppImplHeader
from cppimplcore import CppImplCore
from cppexception import CppException


class CppImpl:
    def __init__(self, api, outputDir):
        apiFileName = os.path.basename(api)
        apiFileName = os.path.splitext(apiFileName)[0]
        skeletonName = 'Skeleton'
        header = os.path.join(outputDir, apiFileName + skeletonName + '.h')
        core = os.path.join(outputDir, apiFileName + skeletonName + '.cpp')
        with open(api, 'r') as stream:
            yamlApi = yaml.safe_load(stream)

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

        self.cppHeader = CppImplHeader(yamlApi, headerFile, [api])
        self.cppCore = CppImplCore(yamlApi, coreFile, header, [api])

    def generate(self):
        self.cppHeader.generate()
        self.cppCore.generate()


def parseCommand():
    parser = argparse.ArgumentParser(description='Generate the implementation skeleton of the service.')
    parser.add_argument('api', help='input api yaml file')
    parser.add_argument('--outputdir', help='output directory of the generated files', required=True)

    return parser.parse_args()


def start():
    args = parseCommand()
    if not args is None:
        impl = CppImpl(args.api, args.outputdir)
        impl.generate()


start()
