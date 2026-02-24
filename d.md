# Show that n log n is O(log n!)

Let logs be in any fixed base greater than 1 (changing base only multiplies by a constant).

We want to show that there exist constants C > 0 and n₀ such that for all n ≥ n₀:

n log n ≤ C log(n!)

---

## Step 1: Expand log(n!)

We use:

log(n!) = Σ_{k=1}^{n} log k

Now take only the second half of the sum:

log(n!) ≥ Σ_{k=⌈n/2⌉}^{n} log k

---

## Step 2: Lower bound each term

For every k in [⌈n/2⌉, n], we have:

k ≥ n/2

So:

log k ≥ log(n/2)

There are at least n/2 such terms, so:

log(n!) ≥ (n/2) log(n/2)

---

## Step 3: Simplify

log(n/2) = log n − log 2

So:

log(n!) ≥ (n/2)(log n − log 2)

For n ≥ 4, we have:

log n ≥ 2 log 2  
⇒ log n − log 2 ≥ (1/2) log n

Therefore:

log(n!) ≥ (n/2)(1/2 log n)  
log(n!) ≥ (n log n)/4

---

## Step 4: Rearrange

n log n ≤ 4 log(n!)   for n ≥ 4

---

## Conclusion

We have shown:

n log n = O(log(n!))

with constants:

C = 4  
n₀ = 4
