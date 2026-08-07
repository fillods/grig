# Contributing to Grig

Thanks for your interest in contributing! This guide covers everything you need to get started.

---

## Table of contents

1. [Getting the code](#getting-the-code)
2. [Building the project](#building-the-project)
3. [Coding conventions](#coding-conventions)
4. [Submitting changes](#submitting-changes)
5. [Translations](#translations)
6. [AI-assisted contributions](#ai-assisted-contributions)
7. [Getting support](#getting-support)

---

## Getting the code

```bash
git clone https://github.com/fillods/grig.git
```

## Building the project

If you obtained the sources from a release tarball:

```bash
./configure
make
sudo make install
```

If you cloned from Git, run `autogen.sh` instead of `configure` — it regenerates the autotools
build files first. Any argument you would pass to `configure` can be passed to `autogen.sh` too:

```bash
./autogen.sh
make
```

**Dependencies** (development packages, not just the runtime libraries):

* `gtk+-2.0` — at least version 2.12.0
* `gthread-2.0` — at least version 2.14.0
* `hamlib` — at least version 4.2

`configure --help` lists the available build options.

---

## Coding conventions

* **Language:** C, built with GNU Autotools (`configure.ac`/`Makefile.am`).
* **Indentation:** 4 spaces, no tabs. Most source files start with an Emacs modeline comment
  (`/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */`) — keep that
  convention in any new file, and match the existing style in files you edit.
* **Comments:** Doxygen-style (`\file`, `\brief`, `\param`, etc.) on functions and file headers,
  matching the existing source.
* **License header:** New source files should carry the same GPLv2 header block used throughout
  `src/` (matching `COPYING`, the license GitHub also lists for this repository).

---

## Submitting changes

1. **Open or comment on an issue first** for anything beyond a trivial fix — it avoids duplicated
   effort and lets the maintainer give early feedback on the approach.
2. **Check for open pull requests** on the same topic before starting.
3. **Fork the repository** and create a branch for your change.
4. **Keep pull requests small and focused** — one concern per PR makes review much faster.
5. **Describe what and why** in the PR body, and note how you tested the change. Grig talks to
   real radio hardware through Hamlib — if you were able to test against real hardware, say so and
   which rig/backend; if not, say that too, so the maintainer knows what still needs a hardware
   check.

---

## Translations

Translations live in `po/` as standard GNU gettext `.po` files, generated from `po/grig.pot`.
To update a translation, edit the relevant `.po` file directly (or run `msgmerge` against a fresh
`.pot` if the source strings have changed) rather than editing compiled output.

---

## AI-assisted contributions

AI tools are welcome for drafting code or documentation, but **every contribution must be
reviewed and understood by the person submitting it** — you should be able to explain how your own
code works, not just that an assistant produced it. If your contribution was substantially
generated or guided by an AI tool, please say so briefly in the PR description — for example:
*"Drafted with an AI assistant, reviewed and tested manually."* This helps the maintainer
understand the provenance of the code and focus review effort appropriately.

---

## Getting support

If you hit a problem using Grig rather than contributing to it, the mailing list and user forums
are linked from the Groundstation project page at SourceForge:

[http://sourceforge.net/projects/groundstation](http://sourceforge.net/projects/groundstation)
