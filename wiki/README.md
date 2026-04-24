# Nadeem Wiki

Project knowledge base for **Nadeem (نديم)** — an NFC audio box for Arabic-speaking children. Wiki built per the methodology in [`../llm-wiki.md`](../llm-wiki.md).

## Start here

- **[AGENTS.md](AGENTS.md)** — schema. Read before editing or querying.
- **[index.md](index.md)** — page catalog.
- **[sources.md](sources.md)** — authoritative source registry.
- **[lint.md](lint.md)** — open contradictions and questions.

## What's here

```
wiki/
├── AGENTS.md          ← schema (the real product)
├── README.md          ← you are here
├── index.md           ← catalog of pages
├── sources.md         ← raw source pointers + authority order
├── lint.md            ← health report
├── pages/
│   ├── system/        ← architecture, glossary, workflows, constraints, status
│   ├── firmware/      ← implemented v0.1 device docs
│   ├── backend/       ← cloud spec (not yet implemented)
│   ├── frontend/      ← web dashboard spec
│   └── mobile/        ← onboarding app spec
├── graph/
│   ├── entities.md    ← typed nodes
│   └── relationships.md ← typed edges
└── decisions/         ← ADRs
```

## What's where (component status)

| Layer | Status | Wiki path |
|-------|--------|-----------|
| Firmware (ESP32-S3) | implemented | [pages/firmware/](pages/firmware/) |
| Backend (Django) | spec-only | [pages/backend/](pages/backend/) |
| Web frontend (Next.js) | spec-only | [pages/frontend/](pages/frontend/) |
| Mobile app (native) | spec-only | [pages/mobile/](pages/mobile/) |

Snapshot: [pages/system/component-status.md](pages/system/component-status.md).

## Methodology

Per [`../llm-wiki.md`](../llm-wiki.md): wiki layers = raw sources + pages + schema. This wiki sits at "minimal viable + structure" on the implementation spectrum:

- ✅ raw sources tracked
- ✅ pages with confidence/status footers
- ✅ schema in `AGENTS.md`
- ✅ entity + relationship graph
- ✅ ADRs
- ⏳ automation hooks (manual today)
- ⏳ hybrid search (low-volume — page count well under 100)
- ⏳ retention decay (manual lint)

## Contributing

When ingesting a new doc:
1. Add to [sources.md](sources.md).
2. Extract entities into [graph/entities.md](graph/entities.md).
3. Extract edges into [graph/relationships.md](graph/relationships.md).
4. Create or update the relevant page under `pages/`.
5. Update [index.md](index.md) if a new page was added.
6. Stamp confidence footer.
7. Resolve contradictions in [lint.md](lint.md).
