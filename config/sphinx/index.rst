Introduction
************

.. container:: twocol

   .. container:: leftside

      .. figure:: images/flecsi-sp.jpg
         :align: left

   .. container:: rightside

      FleCSI-SP, or *FleCSI Specializations* provides various example
      specialzations of the core FleCSI topology types, which may either
      be used as a starting point for developing custom interfaces for
      your application, or, in simple cases, as the interface itself.

      Each subdirectory under the *flecsi-sp* directory in the top-level
      source directory of the project introduces a specialization
      example. There is no particular order in complexity of the
      examples, so users can browse the examples as interest or need
      dictates. As much as possible, each example is written as a
      standalone set of types for easy integration into other projects.

      Check back occaisionally for updates, as we are always developing
      new topologies, and broadening support to provide coverage for new
      applied methods.

.. toctree::
  :caption: Contents:

  src/unstructured
  src/structured
  src/octree
  src/particle

.. Not sure exactly why, but this seems to do what I want at the
   bottom of the page...

.. image:: images/lanl.png
   :align: center
   :scale: 75%
   :target: https://www.lanl.gov

.. container:: twocol

   .. container:: leftside

      .. image:: images/nnsa.png
         :align: left
         :scale: 75%
         :target: https://www.energy.gov/nnsa/national-nuclear-security-administration

   .. container:: rightside

      .. image:: images/ecp.png
         :align: right
         :scale: 75%
         :target: https://www.exascaleproject.org

.. vim: set tabstop=2 shiftwidth=2 expandtab fo=cqt tw=72 :
