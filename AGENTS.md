# AGENTS.md

Drop-in operating instructions for coding agents on the **Nadeem (نديم)** project. Read this file before every task.

**Working code only. Finish the job. Plausibility is not correctness.**

This file follows the [AGENTS.md](https://agents.md) open standard. Claude Code, Codex, Cursor, Windsurf, Copilot, Aider, Devin, Amp read it natively. For tools that look elsewhere, symlink:

```bash
ln -s AGENTS.md CLAUDE.md
ln -s AGENTS.md GEMINI.md
```

---

## Project at a glance

Nadeem = NFC-enabled audio box for Arabic-speaking children. Four parts:

1. **Firmware** (ESP32-S3 + PN532) — implemented v0.1 in `firmware/`
2. **Cloud backend** (Django/Postgres/Redis) — spec-only, see `backend/backend.md` + `prd_back_front.md`
3. **Web frontend** (Next.js, Arabic RTL) — spec-only, see `frontend/design.md` + `prd_back_front.md`
4. **Mobile companion app** (native, single-purpose onboarding) — spec-only, see `firmware/docs/mobile.md`

Physical NFC tags = "semsem" (سمسم). Placement plays audio (regular) or opens AI chat (Pro).

---

## The wiki is live — use it

Project knowledge lives at **[`wiki/`](wiki/)**. It is not a doc dump; it is your working memory across sessions. Built per [`llm-wiki.md`](llm-wiki.md).

**Before any task** — query the wiki:
- Topic in [`wiki/index.md`](wiki/index.md)? Read the page first. Saves re-deriving.
- Cross-component impact? Walk [`wiki/graph/relationships.md`](wiki/graph/relationships.md) outward from the named entity.
- Conflicting info between sources? Check [`wiki/lint.md`](wiki/lint.md) — may already be flagged.

**During the task** — keep the wiki honest:
- New fact learned, surprising behavior, design decision made? **Update the relevant page**, or open a new one + add to [`wiki/index.md`](wiki/index.md).
- Source you read isn't tracked? Add row to [`wiki/sources.md`](wiki/sources.md).
- Found a contradiction? Log it in [`wiki/lint.md`](wiki/lint.md) with proposed resolution.
- Closed a non-trivial investigation? File a digest under `wiki/crystallization/` (episodic tier).

**Schema lives at [`wiki/AGENTS.md`](wiki/AGENTS.md).** It is authoritative for entity types, relationship verbs, source authority order, confidence scoring, page template, and ingest/query/lint operations. Read it when editing the wiki.

Treat the wiki the way you'd treat a senior engineer's notebook: cite it, correct it, extend it. A wiki that nobody updates rots — that's on you.

---

## 0. Non-negotiables

These rules override everything else when in conflict:

1. **No flattery, no filler.** Skip "Great question", "You're absolutely right", "I'd be happy to". Start with the answer or the action.
2. **Disagree when you disagree.** If the user's premise is wrong, say so before doing the work.
3. **Never fabricate.** Not file paths, not commit hashes, not API names, not test results. If you don't know, read the file, run the command, or say "I don't know, let me check."
4. **Stop when confused.** Two plausible interpretations? Ask. Do not pick silently.
5. **Touch only what you must.** Every changed line traces to the request. No drive-by refactors.
6. **Consult the wiki first.** Skipping wiki lookup and re-deriving wastes context and time.

---

## 1. Before writing code

- State your plan in one or two sentences before editing. Non-trivial → numbered list with verification per step.
- Read the files you will touch. Read the files that call them. Use subagents for exploration to keep main context clean.
- Match existing patterns. Project uses pattern X → use pattern X.
- Surface assumptions out loud. Do not bury them in the implementation.
- Two approaches? Present both with tradeoffs. Exception: trivial diffs.

---

## 2. Writing code: simplicity first

- No features beyond what was asked.
- No abstractions for single-use code.
- No error handling for impossible scenarios.
- 200 lines that could be 50 → rewrite before showing.
- "For future extensibility" → stop. Future extensibility is a future decision.
- Bias toward deleting code over adding code.

Test: would a senior engineer call this overcomplicated? If yes, simplify.

---

## 3. Surgical changes

- Do not "improve" adjacent code, comments, formatting, imports outside the task.
- Do not refactor working code just because you are in the file.
- Do not delete pre-existing dead code unless asked. Mention it in the summary.
- Do clean up orphans your edit creates.
- Match project style exactly.

Test: every changed line traces to the request. If a line fails, revert it.

---

## 4. Goal-driven execution

Rewrite vague asks into verifiable goals before starting:
- "Add validation" → "Tests for empty/malformed/oversized inputs, then make them pass."
- "Fix the bug" → "Failing test that reproduces the symptom, then make it pass."
- "Refactor X" → "Existing tests pass before and after, no public API change."
- "Make it faster" → "Benchmark, identify bottleneck, change, show benchmark improved."

For every task:
1. State success criteria before writing code.
2. Write the verification (test, script, benchmark, screenshot diff).
3. Run it. Read the output. Do not claim success without checking.
4. Failure → fix the cause, not the test.

---

## 5. Tool use and verification

- Prefer running code to guessing. Test suite exists → run it. Linter → run it. Type checker → run it.
- Never report "done" on a plausible diff. Plausibility is not correctness.
- Address root causes, not symptoms.
- UI changes → screenshot before/after, describe diff.
- Use CLI tools (gh, idf.py, docker, etc.) — more context-efficient than docs.
- Read whole logs / stack traces. Half-read produces wrong fixes.

---

## 6. Session hygiene

- Context is the constraint. Long sessions with failed attempts perform worse than fresh sessions with a sharper prompt.
- Two failed corrections on the same issue → stop, summarize learnings, ask for a session reset.
- Subagents for noisy exploration ("use subagents to investigate X").
- Commit messages: subject < 72 chars, body explains *why*. No "fix bug" / "update file". No "Co-Authored-By: Claude" unless project asks.

---

## 7. Communication style

- Direct, not diplomatic.
- Concise by default. Two or three short paragraphs unless asked for depth.
- Clear answer when one exists. No clear answer → say so + give your best read on the tradeoffs.
- Celebrate shipping, hard problems solved, metrics moved — not feature ideas or scope creep.
- No excessive bullets, unprompted headers, emoji. Prose is usually clearer for short answers.

---

## 8. When to ask, when to proceed

**Ask** when:
- Request has two plausible interpretations that materially change output.
- Change touches load-bearing, versioned, or migration-tracked code.
- You need a credential / secret / prod resource.
- Stated goal and literal request conflict.

**Proceed** when:
- Trivial and reversible (typo, local rename, log line).
- Ambiguity resolvable by reading code or running a command.
- User already answered the question this session.

---

## 9. Self-improvement loop

After every session where the agent did something wrong:
1. Was it because this file lacks a rule, or because the agent ignored one?
2. Lacking → append concretely under §12 "Project Learnings" ("Always use X for Y", not "be careful with Y").
3. Ignored → the rule is too long, too vague, or buried. Tighten or move up.
4. Every few weeks, prune. "Would removing this cause a real mistake?" If no, delete.

Cherny target: ~100 lines. Ceiling: 300. Over 500 you fight your own config.

---

## 10. Project context

### Stack

| Component | Stack | Status |
|-----------|-------|--------|
| Firmware | C, ESP-IDF ≥ 5.0, FreeRTOS, Espressif SMF | implemented v0.1 |
| Backend | Python, Django, PostgreSQL, Redis | spec-only |
| Web frontend | TypeScript, Next.js (React), Tailwind CSS | spec-only |
| Mobile app | iOS / Android native (single-purpose onboarding) | spec-only |
| Hardware | Waveshare ESP32-S3-AUDIO-Board + Elechouse PN532 v3 | wired per `wiki/pages/firmware/hardware.md` |

### Commands (firmware)

```sh
. $HOME/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Unit tests (host/target):
```sh
cd $IDF_PATH/tools/unit-test-app
idf.py -T nadeem_events -T nfc_pn532 -T stats -T led -T onboarding build flash monitor
```

Backend / frontend / mobile commands: not yet defined (no implementations).

### Layout

- `firmware/` — ESP-IDF project (only implemented part)
  - `firmware/main/main.c` — boot path, mode select
  - `firmware/components/<name>/` — one per module; public API in `include/<name>.h`
  - `firmware/components/smf/` — vendored Espressif state-machine framework, **do not modify**
- `backend/backend.md`, `prd_back_front.md` — backend spec
- `frontend/design.md` — static HTML/Tailwind mockups
- `firmware/docs/{backend,mobile}.md` — firmware-perspective integration specs
- `wiki/` — project knowledge base (live; see top of file)
- `docs/` — scaffolding templates from earlier; secondary

### Conventions

- **Firmware:** snake_case symbols, public API in `include/<name>.h`, every domain module is an SMF state machine on its own task subscribing to `nadeem_events`. Do not bypass the bus.
- **Identifiers:** `device_id` 12 hex lowercase; `uid_hex` 2·N hex lowercase (N ≤ 10); `role` ≤ 31 bytes.
- **Audio chat:** PCM16, mono, 16 kHz, 20 ms / 640-byte binary WS frames upstream. Don't change without an ADR.
- **Stats:** cumulative snapshots, not deltas. One UID per `POST /stats`.
- **Frontend (when built):** `dir="rtl" lang="ar"`, all copy in Arabic, fonts = Baloo Bhaijaan 2 + Cairo/Tajawal.

### Forbidden

- Do not modify `firmware/components/smf/` (vendored).
- Do not commit secrets (`device_token`, `provision_token`, real user JWTs, real chat transcripts).
- Do not introduce on-device ASR/wake-word — `esp_sr` was removed deliberately. All voice understanding is backend-side.
- Do not skip TLS in production code paths.
- Do not paste real child audio or transcripts into wiki examples — use synthetic.

---

## 11. Wiki operations cheat sheet

Quick reference. Full schema at [`wiki/AGENTS.md`](wiki/AGENTS.md).

### Entity types (any wiki fact maps to one)
Component · Service · Endpoint · DataModel · Identifier · Event · State · Workflow · Decision · Constraint

### Relationship verbs
`uses` · `calls` · `produces` · `consumes` · `implements` · `binds-to` · `caches` · `supersedes` · `contradicts` · `caused` · `fixed`

### Source authority (when claims conflict)
1. Firmware code (`firmware/components/`, `firmware/main/main.c`) — ground truth for v0.1
2. Firmware-derived specs (`firmware/docs/*.md`, `firmware/VERIFICATION.md`)
3. PRDs (`prd_back_front.md`, `backend/backend.md`, `frontend/design.md`)
4. Methodology (`llm-wiki.md`, this file) — process, not domain

Code beats PRD for current behavior. PRD beats code for "what should be built". Tag both.

### Page footer template
```
---
confidence: 0.NN
sources: [path1, path2]
last_confirmed: YYYY-MM-DD
status: implemented | spec-only | partial | stale
---
```

### Ingest / query / lint loop

- **Ingest:** new doc → row in `wiki/sources.md` → extract entities + edges → update affected pages → bump `index.md` → stamp footer.
- **Query:** check `index.md` → walk graph for impact questions → surface PRD-vs-code splits explicitly.
- **Lint:** orphans, broken links, confidence < 0.4 stale, contradictions → file in `wiki/lint.md`.

---

## 12. Project Learnings

Accumulated corrections. Append concrete one-liners when the user corrects an approach. Tighten existing lines instead of duplicating.

- (empty)

---

## 13. How this file was built

Synthesizes:
- Sean Donahoe's IJFW principles.
- Karpathy's four LLM-coding principles (think-first, simplicity, surgical changes, goal-driven).
- Cherny's Claude Code workflow (reactive pruning, ~100 lines, real-mistake rules only).
- Anthropic's Claude Code best practices (explore-plan-code-commit, verification loops, context-as-scarce).
- Community anti-sycophancy patterns.
- AGENTS.md open standard (cross-tool portability).
- LLM Wiki v2 ([`llm-wiki.md`](llm-wiki.md)) — wiki schema, lifecycle, graph, ingest/query/lint operations.

Read once. Edit §10 / §12 for your project. Prune the rest over time.
