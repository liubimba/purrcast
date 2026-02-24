interface HeaderProps {
    page: string,
    pages: string[],
    onNavigate: (page: string) => void,
}

export const Header: React.FC<HeaderProps> = ({page, pages, onNavigate}: HeaderProps) => {
    return (
        <div>
            {pages.map(item =>
                <div key={item}>{item}</div>
            )}
        </div>
    )
}