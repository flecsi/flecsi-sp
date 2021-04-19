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
 
    variant('backend', default='legion',
            values=('legion', 'mpi', 'hpx'),
            description='Distributed-Memory Backend', multi=False)

    variant('caliper_detail', default='none',
            values=('none', 'low', 'medium', 'high'),
            description='Set Caliper Profiling Detail', multi=False)

    variant('flog', default=False,
            description='Enable FLOG Logging Utility')    

    variant('unit', default=False,
            description='Enable Unit Tests (Requires +flog)')

    variant('graphviz', default=False,
            description='Enable GraphViz Support')

    variant('hdf5', default=False,
            description='Enable HDF5 Support')

    variant('kokkos', default=False,
            description='Enable Kokkos Support')

    variant('cuda', default=False,
             description='Enable CUDA Support')

    variant('openmp', default=False,
            description='Enable OpenMP Support')

    # Spack-specific variants

    variant('shared', default=True,
            description='Build Shared Libraries')

    #--------------------------------------------------------------------------#
    # Dependencies
    #--------------------------------------------------------------------------#

    depends_on('flecsi@2.0:')
    depends_on('flecsi+flog',when='+flog')
    depends_on('flecsi+flog+unit',when='+unit')
    depends_on('flecsi+graphviz', when='+graphviz')
    depends_on('flecsi+hdf5', when='+hdf5')
    depends_on('flecsi+kokkos', when='+kokkos')
    depends_on('flecsi+cuda', when='+cuda')
    depends_on('flecsi+openp', when='+openmp')
    for b in ('legion', 'mpi', 'hpx'):
        depends_on('flecsi backend={0}'.format(b), when='backend=%s' %b)

    #--------------------------------------------------------------------------#
    # CMake Configuration
    #--------------------------------------------------------------------------#

    def cmake_args(self):
        spec = self.spec
        options = []

        if '~unit' in spec:
            options.append('-DENABLE_UNIT_TESTS=OFF')        

        return options
