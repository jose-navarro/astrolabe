# ASTROLABE

## What is ASTROLABE?

 "The constant irruption of new sensors is a challenge for software systems that do not rely on generic data models able to manage change or innovation. Several data modeling standards exist. Some of these address the problem from a generic perspective but are far too complex for the kind of applications targeted by this work, while others focus strictly on specific kinds of sensors. These approaches pose a problem for the maintainability of software systems dealing with sensor data. This work presents ASTROLABE, a generic and extensible data model specifically devised for trajectory determination systems working with sensors whose error distributions may be fully modeled using means and covariance matrices. A data model relying on four fundamental entities (observation, state, instrument, mathematical model) and related metadata is described; two compliant specifications (for file storage and network communications) are presented; a portable C++ library implementing these specifications is also briefly introduced. ASTROLABE, integrated in CTTC’s trajectory determination system NAVEGA, has been extensively used since 2009 in research and production (real-life) projects, coping successfully with a significant variety of sensors. Such experience helped to improve the data model and validate its suitability for the target problem."

 Excerpted from: Navarro, J.A.; Parés, M.E.; Colomina, I.; Blázquez, M. "ASTROLABE: A Rigorous, Geodetic-Oriented Data Model for Trajectory Determination Systems". ISPRS Int. J. Geo-Inf. 2017, 6, 98.

Thus, ASTROLABE is both a data model for the kind of observations involved in trajectory determination and the portable C++ library that implements the two interfaces it defines.

## The documentation

The folder __docs__ in this distribution contains all the necessary information to understand and use ASTROLABE. There you will find:

* _Two papers_ explaining ASTROLABE. Read this first to get acquainted with the proposed data model. The file name of these papers are:

    * Navarro_Pares_Colomina_Blazques-A generic, extensible data model for trajectory determination systems.pdf.

    * Navarro_Pares_Colomina_Blazquez - ASTROLABE - A rigorous, geodetic-oriented data model for trajectory determination systems.pdf.

* _The Interface Control Document_ (ICD) defining ASTROLABE exhaustively. This is a reference document that may prove harsh to read; that's why it's convenient to read the introductory papers first. The file name of this document is ASTROLABE_1-0_SP_ICD.pdf.

* _Two documents_ explaining:
    * the folder structure of the distribution, what are the dependencies to satisfy, the tools to use and how to build the ASTROLABE library. File name: Building the ASTROLABE library_1-0_NO.pdf.

    * the set of tests / examples that have been included in the distribution to help the developer to understand how the API works. File name: The ASTROLABE tests_1-0_NO.pdf.

Once that the software has been downloaded to your computer, it is possible to build _the doxygen documentation_ for all the classes in the library - that is, the documentation of the API. The guide explaining how to build ASTROLABE also explains how to build this documentation.

## The licence

The MIT License

SPDX short identifier: MIT

Copyright 2018 Centre Tecnològic de Telecomunicacions de Catalunya (CTTC)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
