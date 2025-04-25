function calcularMono() {
    const r = parseFloat(document.getElementById("r1-mono").value);
    const rUnit = parseFloat(document.getElementById("r1-mono-unit").value);
    const c = parseFloat(document.getElementById("c-mono").value);
    const cUnit = parseFloat(document.getElementById("c-mono-unit").value);
  
    const R = r * rUnit;
    const C = c * cUnit;
  
    const T = 1.1 * R * C;
  
    document.getElementById("t-mono").innerText = T.toFixed(6);
}
  
function calcularAstable() {
    const R1 = parseFloat(document.getElementById("r1-astable").value) * 1000; // kΩ
    const R2 = parseFloat(document.getElementById("r2-astable").value) * 1000; // kΩ
    const C = parseFloat(document.getElementById("c-astable").value) * 1e-9;    // nF
  
    const T = 0.693 * (R1 + 2 * R2) * C;
    const f = 1 / T;
    const D = (R1 + R2) / (R1 + 2 * R2) * 100;
  
    document.getElementById("f-astable").innerText = f.toFixed(2);
    document.getElementById("d-astable").innerText = D.toFixed(2);
}