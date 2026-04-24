# Nadeem Wiki — Schema

Operating manual for any LLM ingesting, querying, or extending this wiki. Read before touching wiki files. Pairs with [llm-wiki.md](../llm-wiki.md) (the methodology) and root [AGENTS.md](../AGENTS.md) (engineering rules).

The schema is the real product. This file encodes how Nadeem knowledge is shaped, scored, and kept healthy.

---

## 1. Domain in one paragraph

Nadeem (نديم) is an NFC audio box for Arabic-speaking children. Four parts: (1) **firmware** on Waveshare ESP32-S3-AUDIO-Board with PN532 NFC reader (only part implemented today), (2) **cloud backend** Django/PostgreSQL serving manifests, audio CDN, AI chat WebSocket (PRD only), (3) **web frontend** Next.js parental dashboard, RTL Arabic (PRD only), (4) **mobile companion app** for one-time onboarding (PRD only). Physical NFC tags are "semsems" (سماسم); placing one plays an audio playlist or — if `pro=true` — opens a live AI conversation in a role like "doctor".

---

## 2. Entity ontology

Every fact in this wiki should map to one of these. New entity types require a schema update.

| Type | Examples | Lives in |
|------|----------|----------|
| **Component** (firmware) | `audio`, `semsem`, `hmi`, `led`, `onboarding`, `backend_client`, `bsp`, `nadeem_events`, `nfc_pn532`, `stats` | `pages/firmware/` |
| **Service** (cloud) | backend, CDN, web frontend, mobile app | `pages/backend/`, `pages/frontend/`, `pages/mobile/` |
| **Endpoint** | `POST /bootstrap`, `GET /semsem/{uid}/manifest`, `POST /stats`, `WS /chat`, `GET /info`, `POST /provision` | `pages/backend/contract.md`, `pages/mobile/local-api.md` |
| **DataModel** | User, Device, Semsem, Manifest, UsageStats, ProChatSession | `pages/backend/data-model.md` |
| **Identifier** | `device_id`, `uid_hex`, `provision_token`, `device_token` | `pages/system/glossary.md` |
| **Event** (firmware) | `NEV_HMI_SEMSEM_PLACED`, `NEV_AUDIO_PLAY`, `NEV_SYS_MODE_PRO_ENTER`, … | `pages/firmware/events.md` |
| **State** | `AUDIO_IDLE`, `LED_PRO_LISTENING`, … | `pages/firmware/state-machines.md` |
| **Workflow** | bootstrap, regular playback, pro chat, stats upload | `pages/system/workflows.md` |
| **Decision** | architectural / product calls | `decisions/` |
| **Constraint** | TLS mandatory in prod; RTL only; 16 kHz mono PCM16; tracks ≤ 20 MiB | embedded inline + `pages/system/constraints.md` |

---

## 3. Relationship types

Use these verbs in the graph. Avoid generic "relates to".

- `uses` — A depends on B at runtime
- `calls` — A invokes B's API/endpoint
- `produces` / `consumes` — events, audio frames, stats
- `implements` — component implements spec
- `binds-to` — Device ↔ User after bootstrap
- `caches` — local artifact of remote
- `supersedes` — ADR / decision history
- `contradicts` — flag for resolution
- `caused` / `fixed` — bug or incident chains

---

## 4. Source registry

Every claim must trace to a source. Sources are tracked in [sources.md](sources.md). Authority order when claims conflict:

1. **Firmware code** (`firmware/components/**`, `firmware/main/main.c`) — ground truth for v0.1 device behavior.
2. **Firmware specs derived from code** (`firmware/docs/backend.md`, `firmware/docs/mobile.md`, `firmware/VERIFICATION.md`).
3. **PRD** (`prd_back_front.md`, `backend/backend.md`, `frontend/design.md`) — intent, not yet implemented.
4. **Methodology** (`llm-wiki.md`, root `AGENTS.md`) — process, not domain.

If PRD contradicts firmware: firmware wins for current behavior, PRD wins for "what should be built". Tag both.

---

## 5. Confidence scoring

Each page footer carries a confidence block:

```
---
confidence: 0.NN
sources: [path1, path2]
last_confirmed: YYYY-MM-DD
status: implemented | spec-only | partial | stale
---
```

Rules:
- 2+ source agreement and code presence → 0.9+
- Single PRD source, no code → 0.6
- Inferred / not yet observed → 0.4
- Decay: drop 0.05 per quarter without re-confirmation
- Reset on confirmation (re-reading the source, observing it in code)

---

## 6. Memory tiers

- **Working** — `_inbox/` (drop raw notes here; not yet ingested). Empty by default.
- **Episodic** — session digests under `crystallization/` (one file per major investigation).
- **Semantic** — pages under `pages/` (the wiki proper).
- **Procedural** — `pages/system/workflows.md` (repeatable operational sequences).

Promote upward when evidence accumulates: 3 episodic mentions of the same fact → consolidate into a semantic page.

---

## 7. Operations

### 7.1 Ingest

When a new doc/source lands:
1. Add row to [sources.md](sources.md).
2. Extract entities (§2). New entity → new page or section in existing page.
3. Extract relationships (§3) → update `graph/relationships.md`.
4. If contradicts existing claim → mark with `> ⚠ contradicts: <page>` and open entry in `lint.md`.
5. Update `index.md` if a new page was created.
6. Stamp confidence block.

### 7.2 Query

When asked a question:
1. Check `index.md` for the topic page.
2. Walk `graph/relationships.md` outward from the named entity if impact-style ("what breaks if X changes?").
3. If answer crosses tiers (PRD spec + firmware fact), surface both with their statuses.
4. If answer is novel and worth keeping, file back as a `crystallization/` digest.

### 7.3 Lint

Run periodically (and after every ingest of more than ~3 sources):
- Orphan pages (not in `index.md`)
- Broken wikilinks
- Confidence < 0.4 with no `_inbox/` follow-up
- `status: stale` entries older than 6 months
- Contradictions still unresolved

Findings go into [lint.md](lint.md) with proposed resolution.

---

## 8. Page template

```markdown
# <Title>

> **Type:** <Component|Service|Endpoint|DataModel|Workflow|Decision>
> **Status:** <implemented|spec-only|partial|stale>

## What
One paragraph. What this is, why it exists.

## Surface
APIs / events / pins / fields it exposes.

## Dependencies
Links to other pages it uses or is used by.

## Notes / gotchas
Non-obvious behavior, known limitations.

---
confidence: 0.NN
sources: [...]
last_confirmed: YYYY-MM-DD
status: ...
```

---

## 9. Naming

- Filenames: `kebab-case.md`
- Entity references: backticks for identifiers (`device_token`), bold for proper nouns (**Semsem**, **Semsem-Pro**).
- Arabic text inline: keep raw UTF-8, no transliteration ("سمسم" not "semsem" when quoting product copy).
- Endpoints: full method + path (`POST /bootstrap`, not just "bootstrap").

---

## 10. Privacy & governance

- Firmware streams **child mic audio** over `wss://.../chat`. Treat any chat-session content as sensitive — never paste real transcripts into wiki pages. Use synthetic examples.
- `device_token`, `provision_token`, `user_session_jwt` are secrets. Never inline real values; use `<token>` placeholders.
- Audit trail: every wiki edit is a git commit. Git history is the audit log.

---

## 11. What this wiki is NOT

- Not a tutorial. The README and source files document themselves; the wiki captures cross-cutting facts.
- Not generated docs. Don't auto-export Doxygen here.
- Not a TODO list. Use issues / `_inbox/` for in-flight work.
- Not a copy of the PRD. Cite it, don't duplicate it.

---

## 12. Schema evolution

This file is co-evolved. When adding a new entity type or relationship verb, update §2 / §3 in the same commit that introduces it, and bump confidence on affected pages.
