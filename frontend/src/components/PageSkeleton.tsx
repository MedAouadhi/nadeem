export function PageSkeleton({ cards = 3 }: { cards?: number }) {
  return (
    <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
      {Array.from({ length: cards }).map((_, i) => (
        <div key={i} className="bg-surface-container-highest rounded-xl h-40 animate-pulse" />
      ))}
    </div>
  );
}
