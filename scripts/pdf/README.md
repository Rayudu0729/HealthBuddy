# Submission PDF source

`create-bindu-submission.mjs` and `minipdf.mjs` preserve the source used for the four-page submission snapshot dated 20 October 2026. Current hardware order and implementation status is tracked in `docs/build-tracker.md`.

Run from the repository root with Node.js:

```sh
node scripts/pdf/create-bindu-submission.mjs
```

This overwrites `output/pdf/Bindu-Project.pdf`. It uses no external packages. Review the content and visually inspect the generated PDF before sharing a regenerated version. Temporary page previews are excluded from Git.
