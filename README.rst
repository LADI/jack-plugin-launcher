Abstract
========

JACK Plugin Launcher <https://jack-plugin.ladish.org> is project for
providing improved environment that is well suited for applications in
a JACK Audio Connection Kit modular system.

Jack Plugin is a term for libjack.so linked program that executes
in one or more dedicated processes.

Goals
=====

 * For improved DSP performance and recovery from crashes
   that occured in the UI parts of the Jack Plugins,
   JACK Plugins are facilitated to run with
   separate DSP and UI OS-level processes.
 * Multi-protocol session management approach,like the one used in
   LADISH implementation of liblash and in nsm-proxy used in
   NON Session, RaySession & New Session management systems,
   for handling incompatibilities between session managers
   and apps (out of process plugins) and providing better overal
   experience in modular systems using jack programs for a diverse set
   of sesison management protocols.
 * Possibility for each Jack Plugin to have multiple DSP backends.
   Some projected backend ideas:
    * non-optimized code that runs on any compatible CPU of given ISA
    * optimized code that runs only on matching CPUs
    * code optimized for running on iGPU or dGPU
    * code for running in possible future implementation of jack
      server in kernel mode.
    * Source code to build during load (FAUST, llvm, etc.)
 * Possibility for each Jack Plugin to have multiple UI frontends.
   Some projected frontend ideas:
    * X11 UI
    * ncurses (with optional sixel support) UI, via tmux or screen
    * (HTTP) Web UI
    * GPU accelerated UI for GLX
    * GPU accelerated UI for Wayland
    * GPU accelerated UI for EGL and linux framebuffer
 * Settings for tweaking desired behaviour.
   (Good) built-in defaults and system-wide (/etc), user-wide (~/)
   and per session managment project overrides of the settings.
   Some projected ideas for settings:
    * Order of preference for backends
    * Order of preference for frontends
    * Tweaks for LD_LIBRARY_PATH and other environment variables
    * whether to use tmux, screen or some other terminal emulator
      like xterm (X11) for starting new command-line shell or
      ncruses UI.
 * Execution on other machine in the network, via ssh/libssh/gabriel.
   Facilitate public / private key setup for such deployments.

For installation instructions, read the doc/INSTALL.rst file.
If you are a (linux or libre) distribution packager,
read the doc/PACKAGING.rst file.

Source code
-----------

 * https://gitea.ladish.org/LADI/jpl (Git)
 * https://github.com/LADI/jack-plugin-launcher (GitHub, old, possibly not up-to-date, backup repo)
