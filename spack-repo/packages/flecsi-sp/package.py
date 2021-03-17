# Copyright 2013-2020 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)


from spack import *
import os


class Flecsi(CMakePackage):
    '''FleCSI-SP is a repository for FleCSI specialization examples.
    '''
    homepage = 'http://flecsi.org/'
    git      = 'https://github.com/flecsi/flecsi-sp.git'

    version('2.0', branch='2', submodules=False, preferred=True)

    #--------------------------------------------------------------------------#
    # Dependencies
    #--------------------------------------------------------------------------#

    depends_on('flecsi@2.0:')

    #--------------------------------------------------------------------------#
    # CMake Configuration
    #--------------------------------------------------------------------------#

    def cmake_args(self):
        spec = self.spec
        options = []

        return options
