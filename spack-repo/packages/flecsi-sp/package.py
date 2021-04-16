# Copyright 2013-2020 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


from spack import *
import os


class FlecsiSp(CMakePackage):
    '''FleCSI-SP is a repository for FleCSI specialization examples.
    '''
    homepage = 'http://flecsi.org/'
    git      = 'https://github.com/flecsi/flecsi-sp.git'

    version('2.0', branch='2', submodules=False, preferred=True)

    #--------------------------------------------------------------------------#
    # Variants
    #--------------------------------------------------------------------------#

    variant('flog', default=False,
            description='Enable FLOG Logging Utility')    

    variant('unit', default=False,
            description='Enable Unit Tests (Requires +flog)')

    #--------------------------------------------------------------------------#
    # Dependencies
    #--------------------------------------------------------------------------#

    depends_on('flecsi@2.0:')
    depends_on('flecsi+flog',when='+flog')
    depends_on('flecsi+flog',when='+unit')

    #--------------------------------------------------------------------------#
    # CMake Configuration
    #--------------------------------------------------------------------------#

    def cmake_args(self):
        spec = self.spec
        options = []

        if '~unit' in spec:
            options.append('-DENABLE_UNIT_TESTS=OFF')        

        return options
