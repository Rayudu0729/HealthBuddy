# Submission PDF source

`create-bindu-submission.mjs` and `minipdf.mjs` generate the four-page submission summary reviewed 13 November 2026, using physical test evidence through 1 November. Current progress is tracked in `docs/current-status.md` and the phase contexts. Re-render and inspect after source edits.

Run from the repository root with Node.js:

```sh
node scripts/pdf/create-bindu-submission.mjs
```

This overwrites `output/pdf/Bindu-Project.pdf`. It uses no external packages. Review the content and visually inspect the generated PDF before sharing a regenerated version. Temporary page previews are excluded from Git.
