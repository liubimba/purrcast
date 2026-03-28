import {useLocation, useNavigate} from "react-router-dom";

export const useNavigateWithQuery = () => {
    const navigate = useNavigate();
    const location = useLocation();

    return (path: string) => {
        navigate({
            pathname: path,
            search: location.search
        });
    };
};