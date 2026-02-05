import './TitleWrapper.css'

interface NameWrapperProps {
    title: string;
    children: React.ReactNode;
    className?: string;
    wrapperClassName?: string;
}

export const TitleWrapper: React.FC<NameWrapperProps> = ({
                                                             title,
                                                             children,
                                                             className = "",
                                                             wrapperClassName = ""
                                                         }: NameWrapperProps) => {
    return (
        <div className={`relative ${wrapperClassName}`}>
            <div className="title-heading absolute w-full">
                <h1>{title}</h1>
            </div>
            <div className={`${className} glass`}>
                {children}
            </div>
        </div>

    )
}