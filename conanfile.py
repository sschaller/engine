from conans import ConanFile, CMake

class EngineConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "sdl/2.0.20"  # comma-separated list of requirements
    generators = "cmake", "visual_studio", "txt"
    default_options = {}

    def configure(self):
      self.options['sdl'].vulkan = True

    def imports(self):
      self.copy("*.dll", dst="bin", src="bin")  # From bin to bin
      self.copy("*.pdb", dst="bin", src="bin")  # From bin to bin
      self.copy("*.dylib*", dst="bin", src="lib")  # From lib to bin
        
